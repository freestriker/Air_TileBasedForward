#include "Rendering/RenderFeature/Forward_Opaque_RenderFeature.h"
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
#include "Core/Graphic/Instance/ImageSampler.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderPass>("AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData>("AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature>("AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderPass::Forward_Opaque_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderPass::~Forward_Opaque_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData::Forward_Opaque_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffer(nullptr)
	, needClearColorAttachment(false)
{

}

AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData::~Forward_Opaque_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<Forward_Opaque_RenderPass>())
	, _renderPassName(rttr::type::get<Forward_Opaque_RenderPass>().get_name().to_string())
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

AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::~Forward_Opaque_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<Forward_Opaque_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new Forward_Opaque_RenderFeatureData();
	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);
	featureData->needClearColorAttachment = false;

	return featureData;
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<Forward_Opaque_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<Forward_Opaque_RenderFeatureData*>(renderFeatureData);

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
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
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

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);
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
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().ForwardLightInfosBuffer());
			material->SetSampledImageCube("ambientLightTexture", ambientLightTexture, _sampler);

			commandBuffer->DrawMesh(rendererComponent->mesh, material);
		}
		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::Forward_Opaque_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
