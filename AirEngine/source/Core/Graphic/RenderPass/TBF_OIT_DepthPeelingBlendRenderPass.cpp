#include "Core/Graphic/RenderPass/TBF_OIT_DepthPeelingBlendRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
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
#include "Core/Graphic/RenderPass/TBF_OpaqueRenderPass.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/Memory.h"
#include <array>
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Shader.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/RenderPass/TBF_OIT_DepthPeelingRenderPass.h"
#include "Core/Graphic/Instance/ImageSampler.h"

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
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
	creator.AddDependency(
		"DrawSubpass",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	if (_blendMaterial == nullptr)
	{
		_blendMaterial = new Material(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\OIT_DP_BlendShader.shader"));
	}
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OIT_DepthPeelingBlendCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	auto peeledColorImages = dynamic_cast<TBF_OIT_DepthPeelingRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OIT_DepthPeelingRenderPass")).PeeledColorImages();

	bool needDepthPeelingPass = dynamic_cast<TBF_OIT_DepthPeelingRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OIT_DepthPeelingRenderPass")).NeedDepthPeelingPass();
	if (needDepthPeelingPass)
	{
		///Init attachment layout
		{
			Command::ImageMemoryBarrier colorAttachmentBarrier = Command::ImageMemoryBarrier
			(
				camera->RenderPassTarget()->Attachment("ColorAttachment"),
				VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				{ &colorAttachmentBarrier }
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

		//Blend all layers to color attachment
		{
			_renderCommandBuffer->BeginRenderPass(
				this,
				camera->RenderPassTarget(),
				{ }
			);

			for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
			{
				_blendMaterial->SetSlotData("srcPeeledColorTexture_" + std::to_string(i), {0}, {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _colorTextureSampler->VkSampler_(), peeledColorImages[i]->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});
			}
			_renderCommandBuffer->BindMaterial(_blendMaterial);
			_renderCommandBuffer->DrawMesh(_fullScreenMesh);

			_renderCommandBuffer->EndRenderPass();
		}
	}
	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::TBF_OIT_DepthPeelingBlendRenderPass()
	: RenderPassBase("TBF_OIT_DepthPeelingBlendRenderPass", TBF_OIT_DEPTH_PEELING_BLEND_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _fullScreenMesh(nullptr)
	, _blendMaterial(nullptr)
	, _colorTextureSampler()
{
	_fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");

	_colorTextureSampler = new Instance::ImageSampler
	(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::~TBF_OIT_DepthPeelingBlendRenderPass()
{
}