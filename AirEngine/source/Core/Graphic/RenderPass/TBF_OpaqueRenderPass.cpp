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
#include "Core/Graphic/RenderPass/PreZRenderPass.h"

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
	auto attachmentSize = camera->RenderPassTarget()->Extent();

	VkExtent2D globalGroupSize = { (attachmentSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (attachmentSize.height + TILE_WIDTH - 1) / TILE_WIDTH };
	_opauqeLightIndexListsBuffer = new Instance::Buffer(
		sizeof(LightIndexList) * globalGroupSize.width * globalGroupSize.height + 8, 
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_transparentLightIndexListsBuffer = new Instance::Buffer(
		sizeof(LightIndexList) * globalGroupSize.width * globalGroupSize.height + 8, 
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{

	auto attachmentSize = camera->RenderPassTarget()->Extent();
	VkExtent2D globalGroupSize = { (attachmentSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (attachmentSize.height + TILE_WIDTH - 1) / TILE_WIDTH };

	_renderCommandPool = commandPool;
	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OpaqueCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Clear lists buffer
	_renderCommandBuffer->FillBuffer(_opauqeLightIndexListsBuffer, 0);
	_renderCommandBuffer->FillBuffer(_transparentLightIndexListsBuffer, 0);

	///Wait clear lists buffer finish
	{
		Command::BufferMemoryBarrier opaqueLightListsBarrier = Command::BufferMemoryBarrier
		(
			_opauqeLightIndexListsBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
		);
		Command::BufferMemoryBarrier transparentLightListsBarrier = Command::BufferMemoryBarrier
		(
			_transparentLightIndexListsBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
		);
		_renderCommandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			{ &opaqueLightListsBarrier, &transparentLightListsBarrier }
		);
	}

	auto depthImage = dynamic_cast<PreZRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("PreZRenderPass")).DepthImage();

	_buildLightListsMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	_buildLightListsMaterial->SetUniformBuffer("lightBoundingBoxInfos", CoreObject::Instance::LightManager().TileBasedForwardLightBoundindBoxInfosBuffer());
	_buildLightListsMaterial->SetStorageBuffer("opaqueLightIndexLists", _opauqeLightIndexListsBuffer);
	_buildLightListsMaterial->SetStorageBuffer("transparentLightIndexLists", _transparentLightIndexListsBuffer);
	_buildLightListsMaterial->SetSlotData("depthImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, depthImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
	
	_renderCommandBuffer->BindMaterial(_buildLightListsMaterial);
	_renderCommandBuffer->Dispatch(globalGroupSize.width, globalGroupSize.height, 1);

	//Wait build lists buffer
	{
		Command::BufferMemoryBarrier opaqueLightListsBarrier = Command::BufferMemoryBarrier
		(
			_opauqeLightIndexListsBuffer,
			VK_ACCESS_SHADER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		Command::BufferMemoryBarrier transparentLightListsBarrier = Command::BufferMemoryBarrier
		(
			_opauqeLightIndexListsBuffer,
			VK_ACCESS_SHADER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		_renderCommandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &opaqueLightListsBarrier, &transparentLightListsBarrier }
		);
	}

	///Init attachmnet layout
	{
		Command::ImageMemoryBarrier colorAttachmnetBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("ColorAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ &colorAttachmnetBarrier }
		);

		Command::ImageMemoryBarrier depthAttachmentBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			{ &depthAttachmentBarrier }
		);
	}

	///Render
	{
		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget(),
			{ }
		);

		auto viewMatrix = camera->ViewMatrix();
		auto ambientLightTexture = CoreObject::Instance::LightManager().AmbientTextureCube();
		for (const auto& rendererDistencePair : renderDistanceTable)
		{
			auto& renderer = rendererDistencePair.second;
			auto material = renderer->GetMaterial(Name());

			auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
			if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				//Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
				continue;
			}

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			material->SetTextureCube("ambientLightTexture", ambientLightTexture);
			material->SetStorageBuffer("opaqueLightIndexLists", _opauqeLightIndexListsBuffer);

			_renderCommandBuffer->BindMaterial(material);
			_renderCommandBuffer->DrawMesh(renderer->mesh);
		}
		_renderCommandBuffer->EndRenderPass();
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::OnClear()
{
	delete _opauqeLightIndexListsBuffer;
	delete _transparentLightIndexListsBuffer;

	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::TBF_OpaqueRenderPass()
	: RenderPassBase("TBF_OpaqueRenderPass", TBF_OPAQUE_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _buildLightListsMaterial(nullptr)
	, _opauqeLightIndexListsBuffer(nullptr)
	, _transparentLightIndexListsBuffer(nullptr)
{
	_buildLightListsMaterial = new Material(Core::IO::CoreObject::Instance::AssetManager().Load<Shader>("..\\Asset\\Shader\\BuildTBFLightListsShader.shader"));
}

AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::~TBF_OpaqueRenderPass()
{
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::RenderPass::TBF_OpaqueRenderPass::TransparentLightIndexListsBuffer()
{
	return _transparentLightIndexListsBuffer;
}
