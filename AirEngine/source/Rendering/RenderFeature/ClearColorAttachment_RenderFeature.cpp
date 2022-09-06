#include "Rendering/RenderFeature/ClearColorAttachment_RenderFeature.h"
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

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData>("AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature>("AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData::ClearColorAttachment_RenderFeatureData()
	: RenderFeatureDataBase()
	, clearValue({0, 0, 0, 1})
{

}

AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData::~ClearColorAttachment_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeature()
	: RenderFeatureBase()
{

}

AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::~ClearColorAttachment_RenderFeature()
{

}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	return new ClearColorAttachment_RenderFeatureData();
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	delete static_cast<ClearColorAttachment_RenderFeatureData*>(renderFeatureData);
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<ClearColorAttachment_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Change layout
	{
		auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			camera->attachments["ColorAttachment"],
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
	commandBuffer->ClearColorImage(camera->attachments["ColorAttachment"], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, featureData->clearValue);

	//Change layout
	{
		auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			camera->attachments["ColorAttachment"],
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

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::ClearColorAttachment_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
