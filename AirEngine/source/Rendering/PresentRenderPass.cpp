#include "Rendering/PresentRenderPass.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::PresentRenderPass>("AirEngine::Rendering::PresentRenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

void AirEngine::Rendering::PresentRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"SwapchainAttachment",
		VK_FORMAT_B8G8R8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "SwapchainAttachment" }
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

AirEngine::Rendering::PresentRenderPass::PresentRenderPass()
	: Core::Graphic::Rendering::RenderPassBase()
{
}

AirEngine::Rendering::PresentRenderPass::~PresentRenderPass()
{
}
