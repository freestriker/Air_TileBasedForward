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

#define DEPTH_PEELING_STEP_COUNT 4

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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
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
	if (_blendMaterials.size() != DEPTH_PEELING_STEP_COUNT)
	{
		_blendMaterials.resize(DEPTH_PEELING_STEP_COUNT, nullptr);
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			_blendMaterials[i] = new Material(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\OIT_DP_BlendShader.shader"));
		}
	}
	_peeledColorImages = dynamic_cast<TBF_OIT_DepthPeelingRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OIT_DepthPeelingRenderPass")).PeeledColorImages();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OIT_DepthPeelingBlendCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	bool needDepthPeelingPass = dynamic_cast<TBF_OIT_DepthPeelingRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OIT_DepthPeelingRenderPass")).NeedDepthPeelingPass();

	if (needDepthPeelingPass)
	{
		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget(),
			{ }
		);

		///Wait render finish
		{
			for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
			{
				auto colorAttachmentFinishBarrier = Command::ImageMemoryBarrier
				(
					_peeledColorImages[i],
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &colorAttachmentFinishBarrier }
				);
			}
		}

		//Blend all layers to color attachment
		{
			Command::ImageMemoryBarrier drawBarrier = Command::ImageMemoryBarrier
			(
				camera->RenderPassTarget()->FrameBuffer(Name())->Attachment("ColorAttachment"),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			);

			for (int i = DEPTH_PEELING_STEP_COUNT - 1; i >= 0; i--)
			{
				_blendMaterials[i]->SetSlotData("srcPeeledColorTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _imageSamplers[i]->VkSampler_(), _peeledColorImages[i]->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
				_renderCommandBuffer->BindMaterial(_blendMaterials[i]);
				_renderCommandBuffer->DrawMesh(_fullScreenMesh);

				_renderCommandBuffer->AddPipelineImageBarrier(
					VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					{ &drawBarrier }
				);
			}
		}

		_renderCommandBuffer->EndRenderPass();
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
	_peeledColorImages.clear();
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::TBF_OIT_DepthPeelingBlendRenderPass()
	: RenderPassBase("TBF_OIT_DepthPeelingBlendRenderPass", TBF_OIT_DEPTH_PEELING_BLEND_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _fullScreenMesh(nullptr)
	, _blendMaterials()
	, _peeledColorImages()
	, _imageSamplers()
{
	_fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");

	_imageSamplers.resize(DEPTH_PEELING_STEP_COUNT, nullptr);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		_imageSamplers[i] = new Instance::ImageSampler(VkFilter::VK_FILTER_NEAREST);
	}
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass::~TBF_OIT_DepthPeelingBlendRenderPass()
{
}