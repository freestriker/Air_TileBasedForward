#include "Rendering/RenderFeature/TBForward_Opaque_RenderFeature.h"
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
#include "Rendering/RenderFeature/CSM_ShadowCaster_RenderFeature.h"
#include "Core/Graphic/Instance/ImageSampler.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderPass>("AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData>("AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature>("AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderPass::TBForward_Opaque_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderPass::~TBForward_Opaque_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	settings.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData::TBForward_Opaque_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffer(nullptr)
	, opaqueLightIndexListsBuffer(nullptr)
	, needClearColorAttachment(false)
	, csmShadowMapRenderFeatureData(nullptr)
{

}

AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData::~TBForward_Opaque_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<TBForward_Opaque_RenderPass>())
	, _renderPassName(rttr::type::get<TBForward_Opaque_RenderPass>().get_name().to_string())
	, _sampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{

}

AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::~TBForward_Opaque_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<TBForward_Opaque_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new TBForward_Opaque_RenderFeatureData();
	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);
	featureData->needClearColorAttachment = false;
	struct OcclusionInfo
	{
		alignas(8) glm::vec2 texelSize;
		alignas(4) float intensity;
		alignas(4) float power;
	};
	featureData->occlusionInfo = new Core::Graphic::Instance::Buffer{
			sizeof(OcclusionInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	return featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<TBForward_Opaque_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->occlusionInfo;
	delete featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	struct OcclusionInfo
	{
		alignas(8) glm::vec2 texelSize;
		alignas(4) float intensity;
		alignas(4) float power;
	}; 
	auto&& featureData = static_cast<TBForward_Opaque_RenderFeatureData*>(renderFeatureData); 
	auto&& occlusionInfo = OcclusionInfo{ 
		glm::vec2(1, 1) / glm::vec2(featureData->frameBuffer->Extent2D().width, featureData->frameBuffer->Extent2D().height),
		2.5f,
		5.0f
	};
	featureData->occlusionInfo->WriteData(&occlusionInfo, sizeof(OcclusionInfo));
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<TBForward_Opaque_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (featureData->needClearColorAttachment)
	{
		//Change layout
		{
			auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->frameBuffer->Attachment("ColorAttachment"),
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &colorAttachmentBarrier }
			);
		}

		///Clear
		VkClearColorValue clearValue = { 0, 0, 0, 0 };
		commandBuffer->ClearColorImage(featureData->frameBuffer->Attachment("ColorAttachment"), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearValue);

		//Change layout
		{
			auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->frameBuffer->Attachment("ColorAttachment"),
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				{ &colorAttachmentBarrier }
			);
		}
	}

	///Wait light lists buffer ready
	{
		Core::Graphic::Command::BufferMemoryBarrier opaqueLightListsBarrier = Core::Graphic::Command::BufferMemoryBarrier
		(
			featureData->opaqueLightIndexListsBuffer,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		commandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &opaqueLightListsBarrier }
		);
	}

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);

		CSM_ShadowCaster_RenderFeature::CSM_ShadowCaster_RenderFeatureData* shadowRenderFeatureData = static_cast<CSM_ShadowCaster_RenderFeature::CSM_ShadowCaster_RenderFeatureData*>(featureData->csmShadowMapRenderFeatureData);

		auto viewMatrix = camera->ViewMatrix();
		auto ambientLightTexture = Core::Graphic::CoreObject::Instance::LightManager().AmbientTextureCube();
		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_renderPassName);
			if (material == nullptr) continue;

			auto obbVertexes = rendererComponent->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * rendererComponent->GameObject()->transform.ModelMatrix();
			if (rendererComponent->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				continue;
			}

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			Core::Graphic::CoreObject::Instance::LightManager().SetAmbientLightParameters(material, _sampler);
			material->SetStorageBuffer("opaqueLightIndexLists", featureData->opaqueLightIndexListsBuffer);
			material->SetSampledImage2D("occlusionTexture", featureData->occlusionTexture, _sampler);
			material->SetUniformBuffer("occlusionInfo", featureData->occlusionInfo);

			if (shadowRenderFeatureData)
			{
				shadowRenderFeatureData->SetShadowReceiverMaterialParameters(material);
			}

			commandBuffer->DrawMesh(rendererComponent->mesh, material);
		}
		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::TBForward_Opaque_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
