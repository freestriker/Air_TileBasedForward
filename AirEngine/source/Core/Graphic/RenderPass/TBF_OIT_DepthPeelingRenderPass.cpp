#include "Core/Graphic/RenderPass/TBF_OIT_DepthPeelingRenderPass.h"
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

#define DEPTH_PEELING_STEP_COUNT 4

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	creator.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
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

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	_transparentLightListsBuffer = dynamic_cast<TBF_OpaqueRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OpaqueRenderPass")).TransparentLightIndexListsBuffer();
	
	auto cameraColorImage = camera->attachments["ColorAttachment"];
	auto cameraDepthImage = camera->attachments["DepthAttachment"];

	_depthPeelingThresholdImage = Instance::Image::Create2DImage(
		{ cameraDepthImage->VkExtent3D_().width, cameraDepthImage->VkExtent3D_().height },
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		cameraDepthImage->VkImageAspectFlags_()
	);

	///Create render pass targets
	_renderPassTargets.resize(DEPTH_PEELING_STEP_COUNT, nullptr);
	for (auto& renderPassTarget : _renderPassTargets)
	{
		auto colorImage = Instance::Image::Create2DImage(
			{ cameraColorImage->VkExtent3D_().width, cameraColorImage->VkExtent3D_().height },
			cameraColorImage->VkFormat_(),
			cameraColorImage->VkImageUsageFlags_() | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			cameraColorImage->VkMemoryPropertyFlags_(),
			cameraColorImage->VkImageAspectFlags_()
		);
		auto depthImage = Instance::Image::Create2DImage(
			{ cameraDepthImage->VkExtent3D_().width, cameraDepthImage->VkExtent3D_().height },
			cameraDepthImage->VkFormat_(),
			cameraDepthImage->VkImageUsageFlags_() | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			cameraDepthImage->VkMemoryPropertyFlags_(),
			cameraDepthImage->VkImageAspectFlags_()
		);

		renderPassTarget = CoreObject::Instance::RenderPassManager().GetRenderPassObject(
			{ "TBF_OIT_DepthPeelingRenderPass" },
			{
				{"ColorAttachment", colorImage},
				{"DepthAttachment", depthImage}
			}
		);
	}
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	///Pre cull object and pre bind material
	std::vector< Renderer::Renderer*> targetRenderers = std::vector< Renderer::Renderer*>();
	{
		auto viewMatrix = camera->ViewMatrix();
		for (const auto& rendererPair : renderDistanceTable)
		{
			auto& renderer = rendererPair.second;

			auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
			if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
				continue;
			}
			else
			{
				targetRenderers.emplace_back(renderer);
			}

			renderer->GetMaterial(Name())->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			renderer->GetMaterial(Name())->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
			renderer->GetMaterial(Name())->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			renderer->GetMaterial(Name())->SetTextureCube("ambientLightTexture", CoreObject::Instance::LightManager().AmbientTextureCube());
			renderer->GetMaterial(Name())->SetStorageBuffer("transparentLightIndexLists", _transparentLightListsBuffer);
		}
	}
	_needDepthPeelingPass = targetRenderers.size() > 0;

	_renderCommandPool = commandPool;
	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBF_OIT_DepthPeelingCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Render to layers
	if(_needDepthPeelingPass)
	{
		Command::ImageMemoryBarrier thresholdImageSrcBarrier = Command::ImageMemoryBarrier
		(
			_depthPeelingThresholdImage,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
		);
		Command::ImageMemoryBarrier thresholdImageDstBarrier = Command::ImageMemoryBarrier
		(
			_depthPeelingThresholdImage,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);

		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			///First time populate 0 to _depthPeelingThresholdImage
			if (i == 0)
			{
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &thresholdImageSrcBarrier }
				);

				_renderCommandBuffer->ClearDepthImage(_depthPeelingThresholdImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0);

				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &thresholdImageDstBarrier }
				);
			}
			///Orther time populate preceding depth attachment to _depthPeelingThresholdImage
			else
			{
				Command::ImageMemoryBarrier attachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i - 1]->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &attachmentSrcBarrier, &thresholdImageSrcBarrier }
				);

				_renderCommandBuffer->CopyImage(_renderPassTargets[i - 1]->Attachment("DepthAttachment"), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _depthPeelingThresholdImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				Command::ImageMemoryBarrier attachmentDstBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i - 1]->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					{ &attachmentDstBarrier }
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &thresholdImageDstBarrier }
				);
			}

			///Change current layer's attachment
			{
				Command::ImageMemoryBarrier colorAttachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i]->Attachment("ColorAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					0,
					0
				);
				Command::ImageMemoryBarrier depthAttachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i]->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					0,
					0
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					{ &colorAttachmentSrcBarrier, &depthAttachmentSrcBarrier }
				);
			}

			VkClearValue colorClearValue{};
			VkClearValue depthClearValue{};
			colorClearValue.color = { {0.0f, 0.0f, 0.0f, 0.0f} };
			depthClearValue.depthStencil = { 1.0f, 0 };
			_renderCommandBuffer->BeginRenderPass(
				this,
				_renderPassTargets[i],
				{ colorClearValue, depthClearValue }
			);

			///Copy depth to depth attachment
			{
				Command::ImageMemoryBarrier cameraAttachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					camera->RenderPassTarget()->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
				);
				Command::ImageMemoryBarrier attachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i]->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &cameraAttachmentSrcBarrier , &attachmentSrcBarrier }
				);

				_renderCommandBuffer->CopyImage(camera->RenderPassTarget()->Attachment("DepthAttachment"), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _renderPassTargets[i]->Attachment("DepthAttachment"), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				Command::ImageMemoryBarrier cameraAttachmentDstBarrier = Command::ImageMemoryBarrier
				(
					camera->RenderPassTarget()->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				);
				Command::ImageMemoryBarrier attachmentDstBarrier = Command::ImageMemoryBarrier
				(
					_renderPassTargets[i]->Attachment("DepthAttachment"),
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					{ &cameraAttachmentDstBarrier, &attachmentDstBarrier }
				);
			}

			///Render
			for (const auto& renderer : targetRenderers)
			{
				renderer->GetMaterial(Name())->SetSlotData("depthPeelingThresholdImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, _depthPeelingThresholdImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
				_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
				_renderCommandBuffer->DrawMesh(renderer->mesh);
			}

			_renderCommandBuffer->EndRenderPass();
		}
	}

	///Wait render finish
	if (_needDepthPeelingPass)
	{
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			auto colorAttachmentFinishBarrier = Command::ImageMemoryBarrier
			(
				_renderPassTargets[i]->Attachment("ColorAttachment"),
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

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnClear()
{
	delete _depthPeelingThresholdImage;

	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		delete _renderPassTargets[i]->Attachment("ColorAttachment");
		delete _renderPassTargets[i]->Attachment("DepthAttachment");

		CoreObject::Instance::RenderPassManager().DestroyRenderPassObject(_renderPassTargets[i]);
	}
	_renderPassTargets.clear();

	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);

	_needDepthPeelingPass = false;
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::TBF_OIT_DepthPeelingRenderPass()
	: RenderPassBase("TBF_OIT_DepthPeelingRenderPass", TBF_OIT_DEPTH_PEELING_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _renderPassTargets()
	, _transparentLightListsBuffer(nullptr)
	, _depthPeelingThresholdImage(nullptr)
	, _needDepthPeelingPass(false)
{
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::~TBF_OIT_DepthPeelingRenderPass()
{
}

std::vector<AirEngine::Core::Graphic::Instance::Image*> AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::PeeledColorImages()
{
	std::vector<Instance::Image*> images = std::vector<Instance::Image*>(DEPTH_PEELING_STEP_COUNT, nullptr);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		images[i] = _renderPassTargets[i]->Attachment("ColorAttachment");
	}
	return images;
}

bool AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::NeedDepthPeelingPass()
{
	return _needDepthPeelingPass;
}
