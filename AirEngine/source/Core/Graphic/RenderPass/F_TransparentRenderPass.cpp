#include "Core/Graphic/RenderPass/F_TransparentRenderPass.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Camera/CameraBase.h"
#include "Utils/Log.h"
#include "Asset/Mesh.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/CoreObject/Instance.h"

void AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
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
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	creator.AddDependency(
		"DrawSubpass",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("F_TransparentCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Init attachmnet layout
	{
		Command::ImageMemoryBarrier colorAttachmnetLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("ColorAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ &colorAttachmnetLayoutBarrier }
		);
		Command::ImageMemoryBarrier depthAttachmentLayoutBarrier = Command::ImageMemoryBarrier
		(
			camera->RenderPassTarget()->Attachment("DepthAttachment"),
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
		);

		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			{ &depthAttachmentLayoutBarrier }
		);
	}

	Command::ImageMemoryBarrier drawBarrier = Command::ImageMemoryBarrier
	(
		camera->RenderPassTarget()->FrameBuffer(Name())->Attachment("ColorAttachment"),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);

	///Render
	{
		_renderCommandBuffer->BeginRenderPass(
			this,
			camera->RenderPassTarget()
		);

		auto viewMatrix = camera->ViewMatrix();
		auto ambientLightTexture = CoreObject::Instance::LightManager().AmbientTextureCube();
		for (auto iter = renderDistanceTable.rbegin(); iter != renderDistanceTable.rend(); iter++)
		{
			auto& renderer = iter->second;
			auto material = renderer->GetMaterial(Name());

			auto obbVertexes = renderer->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * renderer->GameObject()->transform.ModelMatrix();
			if (renderer->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				Utils::Log::Message("AirEngine::Core::Graphic::RenderPass::F_TransparentCommandBuffer cull GameObject called " + renderer->GameObject()->name + ".");
				continue;
			}

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", renderer->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", CoreObject::Instance::LightManager().ForwardLightInfosBuffer());
			material->SetTextureCube("ambientLightTexture", ambientLightTexture);

			_renderCommandBuffer->BindMaterial(material);
			_renderCommandBuffer->DrawMesh(renderer->mesh);

			_renderCommandBuffer->AddPipelineImageBarrier(
				VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				{ &drawBarrier }
			);
		}

		_renderCommandBuffer->EndRenderPass();
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::OnClear()
{
	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::F_TransparentRenderPass()
	: RenderPassBase("F_TransparentRenderPass", F_TRANSPARENT_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
{
}

AirEngine::Core::Graphic::RenderPass::F_TransparentRenderPass::~F_TransparentRenderPass()
{
}
