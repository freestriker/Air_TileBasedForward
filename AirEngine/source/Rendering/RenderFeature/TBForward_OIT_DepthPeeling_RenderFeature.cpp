#include "Rendering/RenderFeature/TBForward_OIT_DepthPeeling_RenderFeature.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Asset/Mesh.h"
#include "Renderer/Renderer.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Shader.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderPass>("AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeelingBlend_RenderPass>("AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeelingBlend_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData>("AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature>("AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderPass::TBForward_OIT_DepthPeeling_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderPass::~TBForward_OIT_DepthPeeling_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	settings.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" },
		"DepthAttachment"
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeelingBlend_RenderPass::TBForward_OIT_DepthPeelingBlend_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeelingBlend_RenderPass::~TBForward_OIT_DepthPeelingBlend_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeelingBlend_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" }
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData::TBForward_OIT_DepthPeeling_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffers()
	, colorAttachmentArray(nullptr)
	, depthAttachmentArray()
	, thresholdDepthTexture(nullptr)
	, blendMaterial(nullptr)
	, transparentLightIndexListsBuffer(nullptr)
	, blendFrameBuffer(nullptr)
	, depthPeelingInfoBuffer(nullptr)
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData::~TBForward_OIT_DepthPeeling_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<TBForward_OIT_DepthPeeling_RenderPass>())
	, _renderPassName(rttr::type::get<TBForward_OIT_DepthPeeling_RenderPass>().get_name().to_string())
	, _blendRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<TBForward_OIT_DepthPeelingBlend_RenderPass>())
	, _blendShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_OIT_DepthPeelingBlend_Shader.shader"))
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _textureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::~TBForward_OIT_DepthPeeling_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<TBForward_OIT_DepthPeeling_RenderPass>();
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<TBForward_OIT_DepthPeelingBlend_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\TBF_OIT_DepthPeelingBlend_Shader.shader");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new TBForward_OIT_DepthPeeling_RenderFeatureData();

	return featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<TBForward_OIT_DepthPeeling_RenderFeatureData*>(renderFeatureData);
	
	auto cameraColorImage = camera->attachments["ColorAttachment"];
	auto cameraDepthImage = camera->attachments["DepthAttachment"];

	auto extent = camera->attachments["ColorAttachment"]->VkExtent2D_();

	featureData->depthPeelingInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(DepthPeelingInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	DepthPeelingInfo attachmentSizeInfo = {glm::vec2(1, 1) / glm::vec2(extent.width, extent.height) };
	featureData->depthPeelingInfoBuffer->WriteData(&attachmentSizeInfo, sizeof(DepthPeelingInfo));

	featureData->colorAttachmentArray = Core::Graphic::Instance::Image::Create2DImageArray(
		cameraColorImage->VkExtent2D_(),
		cameraColorImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
		cameraColorImage->VkMemoryPropertyFlags_(),
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		DEPTH_PEELING_STEP_COUNT
	);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		featureData->colorAttachmentArray->AddImageView(
			"SingleImageView_" + std::to_string(i),
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			i,
			1
		);
	}

	featureData->depthAttachmentArray = Core::Graphic::Instance::Image::Create2DImageArray(
		cameraDepthImage->VkExtent2D_(),
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT,
		DEPTH_PEELING_STEP_COUNT
	);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		featureData->depthAttachmentArray->AddImageView(
			"SingleImageView_" + std::to_string(i),
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT,
			i,
			1
		);
	}

	featureData->thresholdDepthTexture = Core::Graphic::Instance::Image::Create2DImage(
		cameraDepthImage->VkExtent2D_(),
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT
	);

	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		featureData->frameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			_renderPass,
			{
				{"ColorAttachment", featureData->colorAttachmentArray},
				{"DepthAttachment", featureData->depthAttachmentArray}
			},
			{
				{"ColorAttachment", "SingleImageView_" + std::to_string(i)},
				{"DepthAttachment", "SingleImageView_" + std::to_string(i)}
			}
		);
	}

	featureData->blendMaterial = new Core::Graphic::Rendering::Material(_blendShader);
	featureData->blendMaterial->SetSampledImage2D("colorTextureArray", featureData->colorAttachmentArray, _textureSampler);

	featureData->blendFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_blendRenderPass,
		{
			{"ColorAttachment", camera->attachments["ColorAttachment"]}
		}
	);
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<TBForward_OIT_DepthPeeling_RenderFeatureData*>(renderFeatureData);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		delete featureData->frameBuffers[i];
	}
	delete featureData->colorAttachmentArray;
	delete featureData->depthAttachmentArray;
	delete featureData->thresholdDepthTexture;
	delete featureData->blendMaterial;
	delete featureData->blendFrameBuffer;
	delete featureData->depthPeelingInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<TBForward_OIT_DepthPeeling_RenderFeatureData*>(renderFeatureData);

	struct TransparentRendererWrapper
	{
		Core::Graphic::Rendering::Material* material;
		Asset::Mesh* mesh;
	};
	std::multimap<float, TransparentRendererWrapper> wrappers = std::multimap<float, TransparentRendererWrapper>();

	///Sort
	{
		auto viewMatrix = camera->ViewMatrix();
		auto ambientLightTexture = Core::Graphic::CoreObject::Instance::LightManager().AmbientTextureCube();
		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_renderPassName);
			if (material == nullptr) continue;
			auto mesh = rendererComponent->mesh;

			auto obbVertexes = mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * rendererComponent->GameObject()->transform.ModelMatrix();
			if (rendererComponent->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix)) continue;

			auto center = mesh->OrientedBoundingBox().Center();
			auto vDepth = (mvMatrix * glm::vec4(center, 1)).z;

			wrappers.emplace(std::abs(vDepth), TransparentRendererWrapper{ material , mesh });

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			material->SetSampledImageCube("ambientLightTexture", ambientLightTexture, _textureSampler);
			material->SetStorageBuffer("transparentLightIndexLists", featureData->transparentLightIndexListsBuffer);
			material->SetSampledImage2D("thresholdDepthTexture", featureData->thresholdDepthTexture, _textureSampler);
			material->SetSampledImage2D("depthTexture", featureData->depthTexture, _textureSampler);
			material->SetUniformBuffer("depthPeelingInfo", featureData->depthPeelingInfoBuffer);
		}
	}

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	if (wrappers.size())
	{
		///Wait light lists buffer ready
		{
			Core::Graphic::Command::BufferMemoryBarrier opaqueLightListsBarrier = Core::Graphic::Command::BufferMemoryBarrier
			(
				featureData->transparentLightIndexListsBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineBufferBarrier(
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &opaqueLightListsBarrier }
			);
		}

		///Render to color attachments
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			///Copy threshold depth texture
			if (i == 0)
			{
				///Init threshold depth texture layout
				{
					auto thresholdDepthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData->thresholdDepthTexture,
						VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
						VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						0,
						VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
						{ &thresholdDepthTextureBarrier }
					);
				}

				commandBuffer->ClearDepthImage(featureData->thresholdDepthTexture, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0);
			}
			else
			{

				///Wait threshold depth ready
				{
					auto thresholdDepthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData->thresholdDepthTexture,
						VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT,
						VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
						{ &thresholdDepthTextureBarrier }
					);
				}

				commandBuffer->CopyImage(featureData->depthAttachmentArray, "SingleImageView_" + std::to_string(i - 1), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, featureData->thresholdDepthTexture, "DefaultImageView", VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			}

			///Wait threshold depth texture ready
			{
				auto thresholdDepthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->thresholdDepthTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &thresholdDepthTextureBarrier }
				);
			}

			///Render
			{
				VkClearValue colorClearValue{};
				VkClearValue depthClearValue{};
				colorClearValue.color = { {0.0f, 0.0f, 0.0f, 0.0f} };
				depthClearValue.depthStencil = { 1.0f, 0 };
				commandBuffer->BeginRenderPass(
					_renderPass,
					featureData->frameBuffers[i],
					{ {"ColorAttachment", colorClearValue}, {"DepthAttachment", depthClearValue} }
				);

				for (const auto& wrapper : wrappers)
				{
					commandBuffer->DrawMesh(wrapper.second.mesh, wrapper.second.material);
				}

				commandBuffer->EndRenderPass();
			}
		}

		///Blend
		{
			auto extent = featureData->blendFrameBuffer->Extent2D();
			BlendInfo info{ glm::vec2(1, 1) / glm::vec2(extent.width, extent.height), DEPTH_PEELING_STEP_COUNT };

			commandBuffer->BeginRenderPass(_blendRenderPass, featureData->blendFrameBuffer);

			commandBuffer->PushConstant(featureData->blendMaterial, VK_SHADER_STAGE_FRAGMENT_BIT, info);
			commandBuffer->DrawMesh(_fullScreenMesh, featureData->blendMaterial);

			commandBuffer->EndRenderPass();
		}

	}
	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
