#include "Core/Graphic/RenderPass/TBF_OIT_AlphaLinkedListRenderPass.h"
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
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/RenderPass/PreZRenderPass.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "glm/glm.hpp"

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::LinkedListHeadImage()
{
	return _linkedListHeadImage;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::PixelColorsBuffer()
{
	return _pixelColorsBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::PixelInfosBuffer()
{
	return _pixelInfosBuffer;
}

bool AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::NeedAlphaLinkedListPass()
{
	while (!_needAlphaLinkedListPass.has_value())
	{
		std::this_thread::yield();
	}
	return _needAlphaLinkedListPass.value();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
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
		{ },
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

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	_linkedListHeadImage = Instance::Image::Create2DImage(
		camera->RenderPassTarget()->Extent(),
		VkFormat::VK_FORMAT_R32_UINT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	auto nodeCount = camera->RenderPassTarget()->Extent().width * camera->RenderPassTarget()->Extent().height * ALPHA_LINKED_LIST_SIZE_FACTOR;

	_pixelColorsBuffer = new Instance::Buffer(
		sizeof(glm::vec4) * nodeCount,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_pixelInfosBuffer = new Instance::Buffer(
		(sizeof(float) + sizeof(uint32_t)) * nodeCount,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	_needAlphaLinkedListPass = std::nullopt;
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	///Pre cull object and pre bind material
	std::vector< Renderer::Renderer*> targetRenderers = std::vector<Renderer::Renderer*>();
	{
		auto viewMatrix = camera->ViewMatrix();
		auto transparentLightListsBuffer = dynamic_cast<TBF_OpaqueRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OpaqueRenderPass")).TransparentLightIndexListsBuffer();
		for (const auto& rendererPair : renderDistanceTable)
		{
			auto& renderer = rendererPair.second;

			auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
			if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
				continue;
			}
			else
			{
				targetRenderers.emplace_back(renderer);
			}

			auto material = renderer->GetMaterial(Name());

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			material->SetTextureCube("ambientLightTexture", CoreObject::Instance::LightManager().AmbientTextureCube());
			material->SetStorageBuffer("transparentLightIndexLists", transparentLightListsBuffer);
			material->SetSlotData("linkedListHeadImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, _linkedListHeadImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
			material->SetStorageBuffer("pixelColors", _pixelColorsBuffer);
			material->SetStorageBuffer("pixelInfos", _pixelInfosBuffer);
			material->SetStorageBuffer("pixelAtomicCounter", _pixelAtomicCounterBuffer);
		}
	}
	_needAlphaLinkedListPass = targetRenderers.size() > 0;

	_renderCommandPool = commandPool;
	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OIT_AlphaLinkedListCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (_needAlphaLinkedListPass)
	{
		///Init current attachment layout
		{
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

		///Init transfer layout
		{
			Command::ImageMemoryBarrier depthAttachmentBarrier = Command::ImageMemoryBarrier
			(
				_linkedListHeadImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &depthAttachmentBarrier }
			);
		}

		///Transfer
		{
			float float0 = 0;
			VkClearColorValue colorValue = {};
			colorValue.uint32[0] = 0;
			colorValue.uint32[1] = 0;
			colorValue.uint32[2] = 0;
			colorValue.uint32[3] = 0;
			_renderCommandBuffer->ClearColorImage(_linkedListHeadImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, colorValue);
			_renderCommandBuffer->FillBuffer(_pixelColorsBuffer, *reinterpret_cast<uint32_t*>(&float0));
			_renderCommandBuffer->FillBuffer(_pixelInfosBuffer, 0);
			_renderCommandBuffer->FillBuffer(_pixelAtomicCounterBuffer, 0);
		}

		///Wait transfer finish
		{
			Command::ImageMemoryBarrier headImageBarrier = Command::ImageMemoryBarrier
			(
				_linkedListHeadImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Command::BufferMemoryBarrier colorsBarrier = Command::BufferMemoryBarrier(
				_pixelColorsBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Command::BufferMemoryBarrier infosBuffer = Command::BufferMemoryBarrier(
				_pixelColorsBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Command::BufferMemoryBarrier atomicCounterBarrier = Command::BufferMemoryBarrier(
				_pixelColorsBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			_renderCommandBuffer->AddPipelineBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &headImageBarrier },
				{ &colorsBarrier , &infosBuffer , &atomicCounterBarrier }
			);
		}

		///Build
		{
			_renderCommandBuffer->BeginRenderPass(
				this,
				camera->RenderPassTarget(),
				{ }
			);

			for (const auto& renderer : targetRenderers)
			{
				_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
				_renderCommandBuffer->DrawMesh(renderer->mesh);
			}

			_renderCommandBuffer->EndRenderPass();
		}

		///Wait build finish
		{
			Command::ImageMemoryBarrier headImageBarrier = Command::ImageMemoryBarrier
			(
				_linkedListHeadImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Command::BufferMemoryBarrier colorsBarrier = Command::BufferMemoryBarrier(
				_pixelColorsBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Command::BufferMemoryBarrier infosBuffer = Command::BufferMemoryBarrier(
				_pixelColorsBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			_renderCommandBuffer->AddPipelineBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &headImageBarrier },
				{ &colorsBarrier , &infosBuffer }
			);
		}
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::OnClear()
{
	delete _linkedListHeadImage;
	delete _pixelColorsBuffer;
	delete _pixelInfosBuffer;
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
	_needAlphaLinkedListPass = std::nullopt;
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::TBF_OIT_AlphaLinkedListRenderPass()
	: RenderPassBase("TBF_OIT_AlphaLinkedListRenderPass", TBF_OIT_ALPHA_LINKED_LIST_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _linkedListHeadImage(nullptr)
	, _pixelColorsBuffer(nullptr)
	, _pixelInfosBuffer(nullptr)
	, _pixelAtomicCounterBuffer(nullptr)
	, _needAlphaLinkedListPass(std::nullopt)
{
	_pixelAtomicCounterBuffer = new Instance::Buffer(
		sizeof(uint32_t), 
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_AlphaLinkedListRenderPass::~TBF_OIT_AlphaLinkedListRenderPass()
{
}
