#include "Rendering/RenderFeature/AO_Cover_RenderFeature.h"
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

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderPass>("AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData>("AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature>("AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderPass::AO_Cover_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderPass::~AO_Cover_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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

AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData::AO_Cover_RenderFeatureData()
	: RenderFeatureDataBase()
	, material(nullptr)
	, frameBuffer(nullptr)
	, occlusionTexture(nullptr)
	, intensity(1.0f)
	, coverInfoBuffer(nullptr)
{

}

AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData::~AO_Cover_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<AO_Cover_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _coverShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\AO_Cover_Shader.shader"))
	, _textureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_LINEAR,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{
	_description = "Used to apply the ambient occlusion effect to the screen.";
}

AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::~AO_Cover_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<AO_Cover_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\AO_Cover_Shader.shader");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new AO_Cover_RenderFeatureData();

	///Frame buffer
	{
		featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, { {"ColorAttachment", camera->attachments["ColorAttachment"]}});
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<AO_Cover_RenderFeatureData*>(renderFeatureData);

	///Cover info
	{
		auto colorAttachmentSize = featureData->frameBuffer->Attachment("ColorAttachment")->VkExtent2D_();
		featureData->coverInfoBuffer = new Core::Graphic::Instance::Buffer(
			sizeof(CoverInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		auto coverInfo = CoverInfo{ glm::vec2(colorAttachmentSize.width, colorAttachmentSize.height) , glm::vec2(1, 1) / glm::vec2(colorAttachmentSize.width, colorAttachmentSize.height), featureData->intensity };
		featureData->coverInfoBuffer->WriteData(&coverInfo, sizeof(CoverInfo));
	}
	
	///Material
	{
		featureData->material = new Core::Graphic::Rendering::Material(_coverShader);
		featureData->material->SetUniformBuffer("coverInfo", featureData->coverInfoBuffer);
		featureData->material->SetSampledImage2D("occlusionTexture", featureData->occlusionTexture, _textureSampler);
	}
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<AO_Cover_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->coverInfoBuffer;
	delete featureData->material;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<AO_Cover_RenderFeatureData*>(renderFeatureData);
	featureData->coverInfoBuffer->WriteData(
		[featureData](void* ptr) -> void
		{
			CoverInfo* coverInfo = reinterpret_cast<CoverInfo*>(ptr);
			coverInfo->intensity = featureData->intensity;
		}
	);
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<AO_Cover_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);

		commandBuffer->DrawMesh(_fullScreenMesh, featureData->material);

		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::AO_Cover_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
