#include "Core/Graphic/RenderPass/PreZRenderPass.h"
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

void AirEngine::Core::Graphic::RenderPass::PreZRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
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
		{ },
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

void AirEngine::Core::Graphic::RenderPass::PreZRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("PreZCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();

	//Render
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Init attachment layout
	{
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
			{ &depthAttachmentFinishBarrier }
		);
	}


	VkClearValue depthClearValue{};
	depthClearValue.depthStencil.depth = 1.0f;
	_renderCommandBuffer->BeginRenderPass(
		this,
		camera->RenderPassTarget(),
		{ depthClearValue }
	);

	auto viewMatrix = camera->ViewMatrix();
	for (const auto& rendererDistencePair : renderDistanceTable)
	{
		auto& renderer = rendererDistencePair.second;
		auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
		auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
		if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
		{
			Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::PreZRenderPass cull GameObject called " + renderer->GameObject()->name + ".");
			continue;
		}

		renderer->GetMaterial(Name())->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		renderer->GetMaterial(Name())->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());

		_renderCommandBuffer->BindMaterial(renderer->GetMaterial(Name()));
		_renderCommandBuffer->DrawMesh(renderer->mesh);
	}
	_renderCommandBuffer->EndRenderPass();
	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::PreZRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::PreZRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer("PreZCommandBuffer");
}

AirEngine::Core::Graphic::RenderPass::PreZRenderPass::PreZRenderPass()
	: RenderPassBase("PreZRenderPass", PRE_Z_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
{
}

AirEngine::Core::Graphic::RenderPass::PreZRenderPass::~PreZRenderPass()
{
}
