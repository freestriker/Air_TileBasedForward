#include "Core/Graphic/RenderPass/GeometryRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Camera/CameraBase.h"
#include "Utils/Log.h"
#include "Asset/Mesh.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"

void AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
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
		{ "NormalAttachment"},
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

void AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	auto attachmentSize = camera->RenderPassTarget()->Extent();

	_depthBuffer = new Instance::Buffer(
		attachmentSize.width * attachmentSize.height * sizeof(float),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_depthImage = Instance::Image::Create2DImage(
		attachmentSize,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("GeometryCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Init attachment layout
	{
		auto normalAttachmentFinishBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("NormalAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			0,
			0
		);
		auto depthAttachmentFinishBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			0,
			0
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			{ &normalAttachmentFinishBarrier, &depthAttachmentFinishBarrier }
		);
	}

	///Render
	{
		VkClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.0f;
		VkClearValue normalClearValue{};
		normalClearValue.color = {0.5f, 0.5f, 0.5f, 1.0f};
		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget(),
			{
				{"NormalAttachment", normalClearValue},
				{"DepthAttachment", depthClearValue}
			}
		);

		auto viewMatrix = camera->ViewMatrix();
		for (const auto& rendererDistencePair : renderDistanceTable)
		{
			auto& renderer = rendererDistencePair.second;
			auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
			if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				//Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::PreZRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
				continue;
			}

			renderer->GetMaterial(Name())->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			renderer->GetMaterial(Name())->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());

			_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
			_renderCommandBuffer->DrawMesh(renderer->mesh);
		}
		_renderCommandBuffer->EndRenderPass();
	}

	///Wait draw finish
	{
		Command::ImageMemoryBarrier depthAttachmentBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &depthAttachmentBarrier }
		);
	}

	//Copy depth to buffer
	_renderCommandBuffer->CopyImageToBuffer(camera->RenderPassTarget()->Attachment("DepthAttachment"), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _depthBuffer);
	
	///Wait copy depth to buffer finish
	{
		Command::ImageMemoryBarrier depthImageLayoutBarrier = Command::ImageMemoryBarrier
		(
			_depthImage,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
		);
		Command::BufferMemoryBarrier depthBufferBarrier = Command::BufferMemoryBarrier
		(
			_depthBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT
		);

		_renderCommandBuffer->AddPipelineBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &depthImageLayoutBarrier },
			{ &depthBufferBarrier }
		);
	}

	///Copy buffer to color
	_renderCommandBuffer->CopyBufferToImage(_depthBuffer, _depthImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	///Wait copy buffer to color finish
	{
		Command::ImageMemoryBarrier depthAttachmentBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_ACCESS_TRANSFER_READ_BIT,
			0
		);
		Command::ImageMemoryBarrier depthImageBarrier = Command::ImageMemoryBarrier
		(
			_depthImage,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			0
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			{ &depthAttachmentBarrier, &depthImageBarrier }
		);
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::OnClear()
{
	delete _depthBuffer;
	delete _depthImage;
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::GeometryRenderPass()
	: RenderPassBase("GeometryRenderPass", GEOMETRY_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _depthBuffer(nullptr)
	, _depthImage(nullptr)
{
}

AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::~GeometryRenderPass()
{
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::DepthImage()
{
	return _depthImage;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::RenderPass::GeometryRenderPass::DepthBuffer()
{
	return _depthBuffer;
}
