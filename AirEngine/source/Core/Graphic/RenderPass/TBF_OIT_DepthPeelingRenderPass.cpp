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
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/RenderPass/PreZRenderPass.h"

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
	auto cameraColorImage = camera->attachments["ColorAttachment"];
	auto cameraDepthImage = camera->attachments["DepthAttachment"];

	for (auto& colorTexture : _colorAttachments)
	{
		colorTexture = Instance::Image::Create2DImage(
			{ cameraColorImage->VkExtent3D_().width, cameraColorImage->VkExtent3D_().height },
			cameraColorImage->VkFormat_(),
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			cameraColorImage->VkMemoryPropertyFlags_(),
			cameraColorImage->VkImageAspectFlags_()
		);
	}
	_thresholdDepthTexture = Instance::Image::Create2DImage(
		{ cameraDepthImage->VkExtent3D_().width, cameraDepthImage->VkExtent3D_().height },
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		cameraDepthImage->VkImageAspectFlags_()
	);
	_depthAttachment = Instance::Image::Create2DImage(
		{ cameraDepthImage->VkExtent3D_().width, cameraDepthImage->VkExtent3D_().height },
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		cameraDepthImage->VkImageAspectFlags_()
	);

	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		_renderPassTargets[i] = CoreObject::Instance::RenderPassManager().GetRenderPassObject(
			{ "TBF_OIT_DepthPeelingRenderPass" },
			{
				{"ColorAttachment", _colorAttachments[i]},
				{"DepthAttachment", _depthAttachment}
			}
		);
	}
}

void AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	///Pre cull object and pre bind material
	std::vector< Renderer::Renderer*> targetRenderers = std::vector<Renderer::Renderer*>();
	{
		auto viewMatrix = camera->ViewMatrix();
		auto transparentLightListsBuffer = dynamic_cast<TBF_OpaqueRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("TBF_OpaqueRenderPass")).TransparentLightIndexListsBuffer();
		auto depthImage = dynamic_cast<PreZRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("PreZRenderPass")).DepthImage();
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

			auto material = renderer->GetMaterial(Name());

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			material->SetTextureCube("ambientLightTexture", CoreObject::Instance::LightManager().AmbientTextureCube());
			material->SetStorageBuffer("transparentLightIndexLists", transparentLightListsBuffer);
			material->SetSlotData("depthPeelingThresholdTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _depthTextureSampler->VkSampler_(), _thresholdDepthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
			material->SetSlotData("depthImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, depthImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
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
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			///First time populate 0 to depth threshold texture
			if (i == 0)
			{
				Command::ImageMemoryBarrier thresholdDepthTextureSrcBarrier = Command::ImageMemoryBarrier
				(
					_thresholdDepthTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &thresholdDepthTextureSrcBarrier }
				);

				_renderCommandBuffer->ClearDepthImage(_thresholdDepthTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0);
			}
			///Orther time copy depth attachment to depth threshold texture
			else
			{
				Command::ImageMemoryBarrier depthAttachmentSrcBarrier = Command::ImageMemoryBarrier
				(
					_depthAttachment,///Use previous depth attachment as depth threshold texture
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &depthAttachmentSrcBarrier }
				);

				Command::ImageMemoryBarrier thresholdDepthTextureSrcBarrier = Command::ImageMemoryBarrier
				(
					_thresholdDepthTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &thresholdDepthTextureSrcBarrier }
				);

				_renderCommandBuffer->CopyImage(_depthAttachment, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _thresholdDepthTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			}

			///Wait transfer threshold depth texture finish
			{
				Command::ImageMemoryBarrier thresholdDepthTextureBarrier = Command::ImageMemoryBarrier
				(
					_thresholdDepthTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &thresholdDepthTextureBarrier }
				);
			}

			///Init current layer's attachment
			{
				Command::ImageMemoryBarrier colorAttachmentBarrier = Command::ImageMemoryBarrier
				(
					_colorAttachments[i],
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					{ &colorAttachmentBarrier }
				);

				Command::ImageMemoryBarrier depthAttachmentBarrier = Command::ImageMemoryBarrier
				(
					_depthAttachment,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				);
				_renderCommandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					{ &depthAttachmentBarrier }
				);
			}

			///Render
			{
				VkClearValue colorClearValue{};
				VkClearValue depthClearValue{};
				colorClearValue.color = { {0.0f, 0.0f, 0.0f, 0.0f} };
				depthClearValue.depthStencil = { 1.0f, 0 };
				_renderCommandBuffer->BeginRenderPass(
					this,
					_renderPassTargets[i],
					{ colorClearValue, depthClearValue }
				);

				for (const auto& renderer : targetRenderers)
				{
					_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
					_renderCommandBuffer->DrawMesh(renderer->mesh);
				}

				_renderCommandBuffer->EndRenderPass();
			}
		}
	}

	///Wait render finish
	if (_needDepthPeelingPass)
	{
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			auto colorAttachmentBarrier = Command::ImageMemoryBarrier
			(
				_colorAttachments[i],
				VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &colorAttachmentBarrier }
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
	delete _thresholdDepthTexture;
	delete _depthAttachment;
	for (const auto& colorTexture : _colorAttachments)
	{
		delete colorTexture;
	}
	for (auto& renderPassTarget : _renderPassTargets)
	{
		CoreObject::Instance::RenderPassManager().DestroyRenderPassObject(renderPassTarget);
	}

	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);

	_needDepthPeelingPass = false;
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::TBF_OIT_DepthPeelingRenderPass()
	: RenderPassBase("TBF_OIT_DepthPeelingRenderPass", TBF_OIT_DEPTH_PEELING_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _renderPassTargets()
	, _needDepthPeelingPass(false)
	, _thresholdDepthTexture(nullptr)
	, _depthAttachment(nullptr)
	, _colorAttachments()
	, _depthTextureSampler(nullptr)
{
	_depthTextureSampler = new Instance::ImageSampler
	(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
	);
}

AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::~TBF_OIT_DepthPeelingRenderPass()
{
}

std::array<AirEngine::Core::Graphic::Instance::Image*, DEPTH_PEELING_STEP_COUNT> AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::PeeledColorImages()
{
	return _colorAttachments;
}

bool AirEngine::Core::Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass::NeedDepthPeelingPass()
{
	return _needDepthPeelingPass;
}
