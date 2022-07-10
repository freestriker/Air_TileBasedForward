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
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" }
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_temporaryImage = Graphic::Instance::Image::Create2DImage(
		camera->RenderPassTarget()->Extent()
		, VkFormat::VK_FORMAT_D32_SFLOAT
		, VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT
		, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT
	);
	_temporaryImageSampler = new Graphic::Instance::ImageSampler
	(
		VkFilter::VK_FILTER_LINEAR,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
	);
	_renderCommandPool = commandPool;
	_renderCommandBuffer = commandPool->CreateCommandBuffer("BackgroundCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();

	//Render
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	if (renderDistanceTable.size() >= 1)
	{
		auto renderer = renderDistanceTable.begin()->second;

		renderer->GetMaterial(Name())->SetSlotData("depthTexture", {0}, {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _temporaryImageSampler->VkSampler_(), _temporaryImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});

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
				_temporaryImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);

			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &depthAttachmentLayoutBarrier, &temporaryImageLayoutBarrier }
			);
		}

		//Copy depth
		{
			_renderCommandBuffer->Blit
			(
				camera->RenderPassTarget()->Attachment("DepthAttachment"),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_temporaryImage,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkFilter::VK_FILTER_NEAREST
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
				VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			);

			Command::ImageMemoryBarrier temporaryImageLayoutBarrier = Command::ImageMemoryBarrier
			(
				_temporaryImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);

			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				{ &depthAttachmentLayoutBarrier }
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &temporaryImageLayoutBarrier }
			);
		}

		//Render background
		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget(),
			{ }
		);
		renderer->GetMaterial(Name())->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
		_renderCommandBuffer->DrawMesh(renderer->mesh);
		_renderCommandBuffer->EndRenderPass();

	}
	if (renderDistanceTable.size() > 1)
	{
		Utils::Log::Exception("Contains multiple background renderer.");
	}
	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnSubmit()
{
	auto opaqueSemaphore = CoreObject::Instance::RenderPassManager().RenderPass("OpaqueRenderPass").Semaphore();
	_renderCommandBuffer->Submit({ opaqueSemaphore }, { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }, { Semaphore() });
}

void AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer("BackgroundCommandBuffer");
	delete _temporaryImageSampler;
	delete _temporaryImage;
}

AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::BackgroundRenderPass()
	: RenderPassBase("BackgroundRenderPass", 5000)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _temporaryImage(nullptr)
	, _temporaryImageSampler(nullptr)
{
}

AirEngine::Core::Graphic::RenderPass::BackgroundRenderPass::~BackgroundRenderPass()
{
}
