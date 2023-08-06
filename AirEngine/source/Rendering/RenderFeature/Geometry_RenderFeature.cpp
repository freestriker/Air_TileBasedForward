#include "Rendering/RenderFeature/Geometry_RenderFeature.h"
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

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderPass>("AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData>("AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::Geometry_RenderFeature>("AirEngine::Rendering::RenderFeature::Geometry_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"DepthTexture",
		VK_FORMAT_R32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	creator.AddColorAttachment(
		"NormalTexture",
		VK_FORMAT_R16G16B16A16_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	creator.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "DepthTexture", "NormalTexture"},
		"DepthAttachment"
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
	);
	creator.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderPass::Geometry_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderPass::~Geometry_RenderPass()
{
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData::Geometry_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffer(nullptr)
	, depthTexture(nullptr)
	, normalTexture(nullptr)
{
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData::~Geometry_RenderFeatureData()
{
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::Geometry_RenderFeature()
	: RenderFeatureBase()
	, _geometryRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<Geometry_RenderPass>())
	, _geometryRenderPassName(rttr::type::get<Geometry_RenderPass>().get_name().to_string())
{
	_description = "Generate depth maps and visual space normal maps.";
}

AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::~Geometry_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<Geometry_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto extent = camera->attachments["ColorAttachment"]->VkExtent2D_();
	auto featureData = new Geometry_RenderFeatureData();
	featureData->depthTexture = Core::Graphic::Instance::Image::Create2DImage(
		extent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->normalTexture = Core::Graphic::Instance::Image::Create2DImage(
		extent,
		VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_geometryRenderPass, 
		{
			{"DepthTexture", featureData->depthTexture},
			{"NormalTexture", featureData->normalTexture},
			{"DepthAttachment", camera->attachments["DepthAttachment"]},
		}
	);
	return featureData;
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<Geometry_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->depthTexture;
	delete featureData->normalTexture;
	delete featureData;
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<Geometry_RenderFeatureData*>(renderFeatureData);
	
	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Render
	{
		VkClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.0f;
		VkClearValue normalClearValue{};
		normalClearValue.color = { 0.5f, 0.5f, 1.0f, 0.0f };
		commandBuffer->BeginRenderPass(
			_geometryRenderPass,
			featureData->frameBuffer,
			{
				{"DepthTexture", depthClearValue},
				{"NormalTexture", normalClearValue},
				{"DepthAttachment", depthClearValue}
			}
		);

		auto viewMatrix = camera->ViewMatrix();
		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_geometryRenderPassName);
			if (material == nullptr) continue;

			auto obbVertexes = rendererComponent->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * rendererComponent->GameObject()->transform.ModelMatrix();
			if (rendererComponent->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix))
			{
				continue;
			}

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());

			commandBuffer->DrawMesh(rendererComponent->mesh, material);
		}
		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::Geometry_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
