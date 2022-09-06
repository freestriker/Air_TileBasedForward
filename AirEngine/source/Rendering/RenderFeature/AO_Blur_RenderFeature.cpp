#include "Rendering/RenderFeature/AO_Blur_RenderFeature.h"
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
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderPass>("AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData>("AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature>("AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderPass::AO_Blur_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderPass::~AO_Blur_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"OcclusionTexture",
		VK_FORMAT_R16_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "OcclusionTexture" }
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData::AO_Blur_RenderFeatureData()
	: RenderFeatureDataBase()
	, horizontalMaterial(nullptr)
	, verticalMaterial(nullptr)
	, horizontalFrameBuffer(nullptr)
	, verticalFrameBuffer(nullptr)
	, horizontalBlurInfoBuffer(nullptr)
	, verticalBlurInfoBuffer(nullptr)
	, temporaryOcclusionTexture(nullptr)
	, occlusionTexture(nullptr)
	, normalTexture(nullptr)
	, sampleOffsetFactor(1.5f)
	, iterateCount(1)
{

}

AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData::~AO_Blur_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<AO_Blur_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _blurShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\AO_Blur_Shader.shader"))
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

}

AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::~AO_Blur_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<AO_Blur_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\AO_Blur_Shader.shader");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new AO_Blur_RenderFeatureData();

	featureData->horizontalMaterial = new Core::Graphic::Rendering::Material(_blurShader);
	featureData->verticalMaterial = new Core::Graphic::Rendering::Material(_blurShader);

	return featureData;
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<AO_Blur_RenderFeatureData*>(renderFeatureData);

	auto occlusionTextureSize = featureData->occlusionTexture->VkExtent2D_();
	///Occlusion texture
	{
		featureData->temporaryOcclusionTexture = Core::Graphic::Instance::Image::Create2DImage(
			occlusionTextureSize,
			VK_FORMAT_R16_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
	}

	///Frame buffer
	{
		featureData->horizontalFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, { {"OcclusionTexture", featureData->temporaryOcclusionTexture} });
		featureData->verticalFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, { {"OcclusionTexture", featureData->occlusionTexture} });
	}

	///Blur info
	{
		featureData->horizontalBlurInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(BlurInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		auto horizontalBlurInfo = BlurInfo{ glm::vec2(occlusionTextureSize.width, occlusionTextureSize.height) , glm::vec2(1, 1) / glm::vec2(occlusionTextureSize.width, occlusionTextureSize.height), glm::vec2(featureData->sampleOffsetFactor, 0) };
		featureData->horizontalBlurInfoBuffer->WriteData(&horizontalBlurInfo, sizeof(BlurInfo));

		featureData->verticalBlurInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(BlurInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		auto verticalBlurInfo = BlurInfo{ glm::vec2(occlusionTextureSize.width, occlusionTextureSize.height) , glm::vec2(1, 1) / glm::vec2(occlusionTextureSize.width, occlusionTextureSize.height), glm::vec2(0, featureData->sampleOffsetFactor) };
		featureData->verticalBlurInfoBuffer->WriteData(&verticalBlurInfo, sizeof(BlurInfo));
	}
	
	///Material
	{
		featureData->horizontalMaterial->SetUniformBuffer("blurInfo", featureData->horizontalBlurInfoBuffer);
		featureData->horizontalMaterial->SetSlotData("normalTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->normalTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
		featureData->horizontalMaterial->SetSlotData("occlusionTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->occlusionTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
		
		featureData->verticalMaterial->SetUniformBuffer("blurInfo", featureData->verticalBlurInfoBuffer);
		featureData->verticalMaterial->SetSlotData("normalTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->normalTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
		featureData->verticalMaterial->SetSlotData("occlusionTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->temporaryOcclusionTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
	}
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<AO_Blur_RenderFeatureData*>(renderFeatureData);
	delete featureData->horizontalMaterial;
	delete featureData->verticalMaterial;
	delete featureData->horizontalFrameBuffer;
	delete featureData->verticalFrameBuffer;
	delete featureData->horizontalBlurInfoBuffer;
	delete featureData->verticalBlurInfoBuffer;
	delete featureData->temporaryOcclusionTexture;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<AO_Blur_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	
	for (int i = 0; i < featureData->iterateCount; i++)
	{
		///Horizontal blur
		{
			commandBuffer->BeginRenderPass(
				_renderPass,
				featureData->horizontalFrameBuffer
			);

			commandBuffer->DrawMesh(_fullScreenMesh, featureData->horizontalMaterial);

			commandBuffer->EndRenderPass();
		}

		///Vertical blur
		{
			commandBuffer->BeginRenderPass(_renderPass, featureData->verticalFrameBuffer);

			commandBuffer->DrawMesh(_fullScreenMesh, featureData->verticalMaterial);

			commandBuffer->EndRenderPass();
		}
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::AO_Blur_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
