#include "Rendering/RenderFeature/GeometryRenderFeature.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderPass>("AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData>("AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeometryRenderFeature>("AirEngine::Rendering::RenderFeature::GeometryRenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"NormalAttachment",
		VK_FORMAT_R16G16B16A16_SFLOAT,
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
		{ "NormalAttachment" },
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

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderPass::GeometryRenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderPass::~GeometryRenderPass()
{
}

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData::GeometryRenderFeatureData()
	: RenderFeatureDataBase()
{
}

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData::~GeometryRenderFeatureData()
{
}

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::GeometryRenderFeature()
	: RenderFeatureBase()
{
	Core::Graphic::CoreObject::Instance::NewRenderPassManager().LoadRenderPass<GeometryRenderPass>();
}

AirEngine::Rendering::RenderFeature::GeometryRenderFeature::~GeometryRenderFeature()
{
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	return new GeometryRenderFeatureData();
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	delete static_cast<GeometryRenderFeatureData*>(renderFeatureData);
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnExcute(Camera::CameraBase* camera, Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, std::vector<Renderer::Renderer*> const* rendererComponents)
{
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
}

void AirEngine::Rendering::RenderFeature::GeometryRenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}
