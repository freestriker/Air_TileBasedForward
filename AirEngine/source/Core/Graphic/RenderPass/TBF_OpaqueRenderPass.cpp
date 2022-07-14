#include "Core/Graphic/RenderPass/TBF_OpaqueRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Camera/CameraBase.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Shader.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Camera/CameraBase.h"
#include "Utils/Log.h"
#include "Asset/Mesh.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Manager/LightManager.h"

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	creator.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" },
		"DepthAttachment"
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
	);
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	auto pixelSize = camera->RenderPassTarget()->Extent();

	_depthImage = Graphic::Instance::Image::Create2DImage(
		pixelSize,
		camera->RenderPassTarget()->Attachment("DepthAttachment")->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT
	);

	VkExtent2D globalGroupSize = { (pixelSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (pixelSize.height + TILE_WIDTH - 1) / TILE_WIDTH };
	_lightIndexListsBuffer = new Instance::Buffer(sizeof(LightIndexList) * globalGroupSize.width * globalGroupSize.height + 8, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{

	auto pixelSize = camera->RenderPassTarget()->Extent();
	VkExtent2D globalGroupSize = { (pixelSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (pixelSize.height + TILE_WIDTH - 1) / TILE_WIDTH };

	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OpaqueCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Change layout
	{
		Command::ImageMemoryBarrier depthAttachmentLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
		);

		Command::ImageMemoryBarrier temporaryImageLayoutBarrier = Command::ImageMemoryBarrier
		(
			_depthImage,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &depthAttachmentLayoutBarrier, &temporaryImageLayoutBarrier }
		);
	}

	//Copy depth
	{
		_renderCommandBuffer->CopyImage
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			_depthImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
	}

	//Change layout
	{
		Command::ImageMemoryBarrier depthAttachmentLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
		);

		Command::ImageMemoryBarrier temporaryImageLayoutBarrier = Command::ImageMemoryBarrier
		(
			_depthImage,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			{ &depthAttachmentLayoutBarrier }
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			{ &temporaryImageLayoutBarrier }
		);
	}

	//clear buffer
	_renderCommandBuffer->FillBuffer(_lightIndexListsBuffer, 0);
	{
		Command::BufferMemoryBarrier bufferClearEndBarrier = Command::BufferMemoryBarrier
		(
			_lightIndexListsBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
		);
		_renderCommandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			{ &bufferClearEndBarrier }
		);
	}

	_buildLightListsMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	_buildLightListsMaterial->SetUniformBuffer("lightBoundingBoxInfos", CoreObject::Instance::LightManager().TileBasedForwardLightBoundindBoxInfosBuffer());
	_buildLightListsMaterial->SetStorageBuffer("lightIndexLists", _lightIndexListsBuffer);
	_buildLightListsMaterial->SetSlotData("depthImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, _depthImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });

	_renderCommandBuffer->BindMaterial(_buildLightListsMaterial);
	_renderCommandBuffer->Dispatch(globalGroupSize.width, globalGroupSize.height, 1);

	//finish build lists buffer
	{
		Command::BufferMemoryBarrier bufferClearEndBarrier = Command::BufferMemoryBarrier
		(
			_lightIndexListsBuffer,
			VK_ACCESS_SHADER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		_renderCommandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &bufferClearEndBarrier }
		);
	}

	_renderCommandBuffer->BeginRenderPass(
		this,
		camera->RenderPassTarget(),
		{ }
	);

	auto viewMatrix = camera->ViewMatrix();
	for (const auto& rendererDistencePair : renderDistanceTable)
	{
		auto& renderer = rendererDistencePair.second;
		auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
		auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
		if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
		{
			Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
			continue;
		}

		renderer->GetMaterial(Name())->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		renderer->GetMaterial(Name())->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
		renderer->GetMaterial(Name())->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
		renderer->GetMaterial(Name())->SetTextureCube("ambientLightTexture", _ambientLightTexture);
		renderer->GetMaterial(Name())->SetStorageBuffer("lightIndexLists", _lightIndexListsBuffer);

		_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
		_renderCommandBuffer->DrawMesh(renderer->mesh);
	}
	_renderCommandBuffer->EndRenderPass();


	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnClear()
{
	delete _lightIndexListsBuffer;
	delete _depthImage;
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::TBF_OpaqueRenderPass()
	: RenderPassBase("TBF_OpaqueRenderPass", TBF_OPAQUE_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _buildLightListsMaterial(nullptr)
	, _lightIndexListsBuffer(nullptr)
	, _depthImage(nullptr)
	, _ambientLightTexture(nullptr)
{
	auto buildLightListsShader = Core::IO::CoreObject::Instance::AssetManager().Load<Shader>("..\\Asset\\Shader\\TBFBuildLightListsShader.shader");
	_buildLightListsMaterial = new Material(buildLightListsShader);
	_ambientLightTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
}

AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::~TBF_OpaqueRenderPass()
{
}
