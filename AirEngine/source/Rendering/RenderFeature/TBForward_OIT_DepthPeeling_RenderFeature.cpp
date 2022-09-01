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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
	);
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData::TBForward_OIT_DepthPeeling_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffers()
	, colorAttachments()
	, depthAttachments()
	, thresholdDepthTexture(nullptr)
	, blendMaterial(nullptr)
	, transparentLightIndexListsBuffer(nullptr)
	, blendFrameBuffer(nullptr)
	, attachmentSizeInfoBuffer(nullptr)
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
	auto featureData = new TBForward_OIT_DepthPeeling_RenderFeatureData();
	
	auto cameraColorImage = camera->attachments["ColorAttachment"];
	auto cameraDepthImage = camera->attachments["DepthAttachment"];

	auto extent = camera->attachments["ColorAttachment"]->VkExtent2D_();

	featureData->attachmentSizeInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(AttachmentSizeInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	AttachmentSizeInfo attachmentSizeInfo = {glm::vec2(extent.width, extent.height), glm::vec2(1, 1) / glm::vec2(extent.width, extent.height) };
	featureData->attachmentSizeInfoBuffer->WriteData(&attachmentSizeInfo, sizeof(AttachmentSizeInfo));

	for (auto& colorAtttachment : featureData->colorAttachments)
	{
		colorAtttachment = Core::Graphic::Instance::Image::Create2DImage(
			cameraColorImage->VkExtent2D_(),
			cameraColorImage->VkFormat_(),
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			cameraColorImage->VkMemoryPropertyFlags_(),
			cameraColorImage->VkImageAspectFlags_()
		);
	}
	for (auto& depthAttachment : featureData->depthAttachments)
	{
		depthAttachment = Core::Graphic::Instance::Image::Create2DImage(
			cameraDepthImage->VkExtent2D_(),
			cameraDepthImage->VkFormat_(),
			VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			cameraDepthImage->VkMemoryPropertyFlags_(),
			cameraDepthImage->VkImageAspectFlags_()
		);
	}
	featureData->thresholdDepthTexture = Core::Graphic::Instance::Image::Create2DImage(
		cameraDepthImage->VkExtent2D_(),
		cameraDepthImage->VkFormat_(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		cameraDepthImage->VkMemoryPropertyFlags_(),
		cameraDepthImage->VkImageAspectFlags_()
	);

	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		featureData->frameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			_renderPass,
			{
				{"ColorAttachment", featureData->colorAttachments[i]},
				{"DepthAttachment", featureData->depthAttachments[i]}
			}
		);
	}

	featureData->blendMaterial = new Core::Graphic::Rendering::Material(_blendShader);
	for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
	{
		featureData->blendMaterial->SetSlotData("colorTexture_" + std::to_string(i), { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->colorAttachments[i]->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});
	}
	featureData->blendMaterial->SetUniformBuffer("attachmentSizeInfo", featureData->attachmentSizeInfoBuffer);

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
		delete featureData->colorAttachments[i];
		delete featureData->frameBuffers[i];
		delete featureData->depthAttachments[i];
	}
	delete featureData->thresholdDepthTexture;
	delete featureData->blendMaterial;
	delete featureData->attachmentSizeInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<TBForward_OIT_DepthPeeling_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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
			material->SetTextureCube("ambientLightTexture", Core::Graphic::CoreObject::Instance::LightManager().AmbientTextureCube());
			material->SetStorageBuffer("transparentLightIndexLists", featureData->transparentLightIndexListsBuffer);
			material->SetSlotData("thresholdDepthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->thresholdDepthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
			material->SetSlotData("depthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, featureData->depthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
			material->SetUniformBuffer("attachmentSizeInfo", featureData->attachmentSizeInfoBuffer);
		}
	}

	///Wait depth texture ready
	{
		auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			featureData->depthTexture,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		commandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &colorAttachmentBarrier }
		);
	}

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
			commandBuffer->ClearDepthImage(featureData->thresholdDepthTexture, VK_IMAGE_LAYOUT_GENERAL, 0);
		}
		else
		{
			///Wait depth ready
			{
				auto thresholdDepthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->depthAttachments[i - 1],
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &thresholdDepthTextureBarrier }
				);
			}

			commandBuffer->CopyImage(featureData->depthAttachments[i - 1], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, featureData->thresholdDepthTexture, VK_IMAGE_LAYOUT_GENERAL);
		}

		///Wait threshold depth texture ready
		{
			auto thresholdDepthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->thresholdDepthTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
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

	///Wait color attachments ready
	{
		for (int i = 0; i < DEPTH_PEELING_STEP_COUNT; i++)
		{
			auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->colorAttachments[i],
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &colorAttachmentBarrier }
			);
		}
	}

	///Wait camera color attachment ready
	{
		auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			featureData->blendFrameBuffer->Attachment("ColorAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		);
		commandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ &colorAttachmentBarrier }
		);
	}

	///Blend
	{
		commandBuffer->BeginRenderPass(
			_blendRenderPass,
			featureData->blendFrameBuffer
		);

		for (const auto& wrapper : wrappers)
		{
			commandBuffer->DrawMesh(_fullScreenMesh, featureData->blendMaterial);
		}

		commandBuffer->EndRenderPass();
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
