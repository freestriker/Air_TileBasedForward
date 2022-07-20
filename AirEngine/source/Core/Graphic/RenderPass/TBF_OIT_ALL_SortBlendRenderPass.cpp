#include "Core/Graphic/RenderPass/TBF_OIT_ALL_SortBlendRenderPass.h"
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
#include "Core/Graphic/RenderPass/TBF_OIT_AlphaLinkedListRenderPass.h"

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
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

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	if (_blendMaterial == nullptr)
	{
		_blendMaterial = new Material(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\OIT_ALL_SortBlendShader.shader"));
	}
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OIT_ALL_SortBlendCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	auto& renderPass = dynamic_cast<TBF_OIT_AlphaLinkedListRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OIT_AlphaLinkedListRenderPass"));
	bool needAlphaLinkedListPass = renderPass.NeedAlphaLinkedListPass();
	if (needAlphaLinkedListPass)
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
		}

		auto headImage = renderPass.LinkedListHeadImage();
		auto colorsBuffer = renderPass.PixelColorsBuffer();
		auto infosBuffer = renderPass.PixelInfosBuffer();

		///Wait build finish
		{
			Command::ImageMemoryBarrier headImageBarrier = Command::ImageMemoryBarrier
			(
				headImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Command::BufferMemoryBarrier colorsBarrier = Command::BufferMemoryBarrier(
				colorsBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Command::BufferMemoryBarrier infosBarrier = Command::BufferMemoryBarrier(
				infosBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			_renderCommandBuffer->AddPipelineBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &headImageBarrier },
				{ &colorsBarrier , &infosBarrier }
			);
		}

		//Blend
		{
			_renderCommandBuffer->BeginRenderPass(
				this,
				camera->RenderPassTarget(),
				{ }
			);

			_blendMaterial->SetSlotData("linkedListHeadImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, headImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
			_blendMaterial->SetStorageBuffer("pixelColors", colorsBuffer);
			_blendMaterial->SetStorageBuffer("pixelInfos", infosBuffer);

			_renderCommandBuffer->BindMaterial(_blendMaterial);
			_renderCommandBuffer->DrawMesh(_fullScreenMesh);

			_renderCommandBuffer->EndRenderPass();
		}
	}
	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::TBF_OIT_ALL_SortBlendRenderPass()
	: RenderPassBase("TBF_OIT_ALL_SortBlendRenderPass", TBF_OIT_ALL_SORT_BLEND_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _fullScreenMesh(nullptr)
	, _blendMaterial(nullptr)
{
	_fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_ALL_SortBlendRenderPass::~TBF_OIT_ALL_SortBlendRenderPass()
{
}
