#include "Core/Graphic/RenderPass/BackgroundRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Utils/Log.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Camera/CameraBase.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/Mesh.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Shader.h"

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
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

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	if (_material == nullptr)
	{
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\BackgroundShader.shader");
		_material = new Core::Graphic::Material(shader);
	}
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;
	_renderCommandBuffer = commandPool->CreateCommandBuffer("BackgroundCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Init attachmnet layout
	{
		Command::ImageMemoryBarrier colorAttachmnetLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("ColorAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ &colorAttachmnetLayoutBarrier }
		);
		Command::ImageMemoryBarrier depthAttachmentLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			{ &depthAttachmentLayoutBarrier }
		);
	}

	///Render background
	{

		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget(),
			{ }
		);

		_material->SetTextureCube("backgroundTexture", _backgroundTexture);
		_material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		//_material->SetSlotData("depthImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, _temporaryDepthImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
		_renderCommandBuffer->BindMaterial(_material);
		_renderCommandBuffer->DrawMesh(_fullScreenMesh);

		_renderCommandBuffer->EndRenderPass();
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::BackgroundRenderPass()
	: RenderPassBase("BackgroundRenderPass", BACKGROUND_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _material(nullptr)
	, _fullScreenMesh(nullptr)
	, _backgroundTexture(nullptr)
{
	_fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
	_backgroundTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
}

AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::~BackgroundRenderPass()
{
}
