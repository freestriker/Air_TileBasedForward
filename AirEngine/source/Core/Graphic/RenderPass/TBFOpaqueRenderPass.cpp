#include "Core/Graphic/RenderPass/TBFOpaqueRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"

void AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
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
		{ "ColorAttachment" },
		"DepthAttachment"
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Manager::RenderPassTarget* renderPassObject)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("TBFOpaqueCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();

	auto depthSize = renderPassObject->FrameBuffer(Name())->Attachment("DepthAttachment")->VkExtent3D_();
	auto depthFormat = renderPassObject->FrameBuffer(Name())->Attachment("DepthAttachment")->VkFormat_();
	_depthStorageBuffer = new Instance::Buffer(depthSize.width * depthSize.height * 4, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthFormat);

	//Render
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	//Render queue attachment to attachment layout
	{
		Command::ImageMemoryBarrier depthAttachmentStartCopyBarrier = Command::ImageMemoryBarrier
		(
			renderPassObject->FrameBuffer(Name())->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &depthAttachmentStartCopyBarrier }
		);

		Command::BufferMemoryBarrier depthBufferStartCopyBarrier = Command::BufferMemoryBarrier
		(
			_depthStorageBuffer,
			0,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
		);

		_renderCommandBuffer->AddPipelineBufferBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &depthBufferStartCopyBarrier }
		);
	}
	_renderCommandBuffer->CopyImageToBuffer(renderPassObject->FrameBuffer(Name())->Attachment("DepthAttachment"), _depthStorageBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	{
		Command::ImageMemoryBarrier depthAttachmentEndCopyBarrier = Command::ImageMemoryBarrier
		(
			renderPassObject->FrameBuffer(Name())->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT,
			0
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			{ &depthAttachmentEndCopyBarrier }
		);
		
		Command::BufferMemoryBarrier depthBufferEndCopyBarrier = Command::BufferMemoryBarrier
		(
			_depthStorageBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		_renderCommandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			{ &depthBufferEndCopyBarrier }
		);
	}
	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::OnClear()
{
	delete _depthStorageBuffer;
	_renderCommandPool->DestoryCommandBuffer("TBFOpaqueCommandBuffer");
}

AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::TBFOpaqueRenderPass()
	: RenderPassBase("TBFOpaqueRenderPass", 2200)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _depthStorageBuffer(nullptr)
{
}

AirEngine::Core::Graphic::RenderPass::TBFOpaqueRenderPass::~TBFOpaqueRenderPass()
{
}
