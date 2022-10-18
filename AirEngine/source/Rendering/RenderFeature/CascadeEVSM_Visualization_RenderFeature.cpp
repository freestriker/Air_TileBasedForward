#include "Rendering/RenderFeature/CascadeEVSM_Visualization_RenderFeature.h"
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
#include <map>
#include <random>
#include "Utils/RandomSphericalCoordinateGenerator.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Rendering/RenderFeature/Geometry_RenderFeature.h"
#include "Rendering/RenderFeature/CascadeEVSM_ShadowCaster_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderPass>("AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeatureData>("AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature>("AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderPass::CascadeEVSM_Visualization_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderPass::~CascadeEVSM_Visualization_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" }
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeatureData::CascadeEVSM_Visualization_RenderFeatureData()
	: RenderFeatureDataBase()
	, visualizationMaterial(nullptr)
	, visualizationFrameBuffer(nullptr)
	, visualizationInfoBuffer(nullptr)
	, cascadeEvsmRenderFeatureData(nullptr)
	, geometryRenderFeatureData(nullptr)
{

}

AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeatureData::~CascadeEVSM_Visualization_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<CascadeEVSM_Visualization_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _visualizationShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\CascadeEVSM_Visualization_Shader.shader"))
	, _textureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{

}

AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::~CascadeEVSM_Visualization_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<CascadeEVSM_Visualization_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\CascadeEVSM_Visualization_Shader.shader");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new CascadeEVSM_Visualization_RenderFeatureData();

	return featureData;
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<CascadeEVSM_Visualization_RenderFeatureData*>(renderFeatureData);

	///Frame buffer
	{
		featureData->visualizationFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, { {"ColorAttachment", camera->attachments["ColorAttachment"]} });
	}

	///Visualization info
	{
		auto colorAttachmentSize = featureData->visualizationFrameBuffer->Attachment("ColorAttachment")->VkExtent2D_();
		featureData->visualizationInfoBuffer = new Core::Graphic::Instance::Buffer(
			sizeof(VisualizationInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		auto coverInfo = VisualizationInfo{ glm::vec2(colorAttachmentSize.width, colorAttachmentSize.height) , glm::vec2(1, 1) / glm::vec2(colorAttachmentSize.width, colorAttachmentSize.height) };
		featureData->visualizationInfoBuffer->WriteData(&coverInfo, sizeof(VisualizationInfo));
	}

	///Material
	{
		featureData->visualizationMaterial = new Core::Graphic::Rendering::Material(_visualizationShader);
		featureData->visualizationMaterial->SetUniformBuffer("visualizationInfo", featureData->visualizationInfoBuffer);
		featureData->visualizationMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());

		Geometry_RenderFeature::Geometry_RenderFeatureData* geometryData = static_cast<Geometry_RenderFeature::Geometry_RenderFeatureData*>(featureData->geometryRenderFeatureData);
		featureData->visualizationMaterial->SetSampledImage2D("depthTexture", geometryData->depthTexture, _textureSampler);

		CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData* cascadeEvsmData = static_cast<CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData*>(featureData->cascadeEvsmRenderFeatureData);
		cascadeEvsmData->SetShadowReceiverMaterialParameters(featureData->visualizationMaterial);
	}
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<CascadeEVSM_Visualization_RenderFeatureData*>(renderFeatureData);

	delete featureData->visualizationMaterial;
	delete featureData->visualizationFrameBuffer;
	delete featureData->visualizationInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<CascadeEVSM_Visualization_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->visualizationFrameBuffer);

		commandBuffer->DrawMesh(_fullScreenMesh, featureData->visualizationMaterial);

		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_Visualization_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
