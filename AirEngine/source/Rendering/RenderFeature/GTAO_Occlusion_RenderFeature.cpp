#include "Rendering/RenderFeature/GTAO_Occlusion_RenderFeature.h"
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
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderPass>("AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeatureData>("AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature>("AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderPass::GTAO_Occlusion_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderPass::~GTAO_Occlusion_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeatureData::GTAO_Occlusion_RenderFeatureData()
	: RenderFeatureDataBase()
	, material(nullptr)
	, frameBuffer(nullptr)
	, occlusionTexture(nullptr)
	, gtaoInfoBuffer(nullptr)
	, noiseTexture(nullptr)
	, noiseStagingBuffer(nullptr)
	, sampleRadius(0.2f)
	, sampleBiasAngle(30.0f)
	, stepCount(4)
	, sliceCount(4)
	, noiseTextureWidth(64)
	, depthTexture(nullptr)
{

}

AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeatureData::~GTAO_Occlusion_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GTAO_Occlusion_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _gtaoShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GTAO_Occlusion_Shader.shader"))
	, _textureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_LINEAR,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{

}

AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::~GTAO_Occlusion_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GTAO_Occlusion_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\GTAO_Occlusion_Shader.shader");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new GTAO_Occlusion_RenderFeatureData();
	VkExtent2D extent = { camera->attachments["ColorAttachment"]->VkExtent2D_().width / 2, camera->attachments["ColorAttachment"]->VkExtent2D_().height / 2 };

	///Occlusion texture
	{
		featureData->occlusionTexture = Core::Graphic::Instance::Image::Create2DImage(
			extent,
			VK_FORMAT_R16_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
	}

	///Frame buffer
	{
		featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, { {"OcclusionTexture", featureData->occlusionTexture} });
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<GTAO_Occlusion_RenderFeatureData*>(renderFeatureData);
	VkExtent2D extent = { camera->attachments["ColorAttachment"]->VkExtent2D_().width / 2, camera->attachments["ColorAttachment"]->VkExtent2D_().height / 2 };

	///Noise texture
	{
		featureData->noiseTexture = Core::Graphic::Instance::Image::Create2DImage(
			{ static_cast<uint32_t>(featureData->noiseTextureWidth), static_cast<uint32_t>(featureData->noiseTextureWidth) },
			VK_FORMAT_R32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
	}

	///Occlusion texture size info
	{
		featureData->gtaoInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(GTAOInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		featureData->gtaoInfoBuffer->WriteData(
			[featureData, extent](void* ptr)->void
		{
			GTAOInfo* gtaoInfo = reinterpret_cast<GTAOInfo*>(ptr);
			gtaoInfo->attachmentize = glm::vec2(extent.width, extent.height);
			gtaoInfo->attachmentTexelSize = glm::vec2(1, 1) / gtaoInfo->attachmentize;
			gtaoInfo->sampleRadius = featureData->sampleRadius;
			gtaoInfo->sampleBiasAngle = featureData->sampleBiasAngle;
			gtaoInfo->stepCount = featureData->stepCount;
			gtaoInfo->sliceCount = featureData->sliceCount;
			gtaoInfo->noiseTextureWidth = featureData->noiseTextureWidth;
		}
		);
	}

	///Build noise staging buffer
	{
		featureData->noiseStagingBuffer = new Core::Graphic::Instance::Buffer{
				sizeof(float) * featureData->noiseTextureWidth * featureData->noiseTextureWidth,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		std::vector<float> noiseInfo = std::vector<float>(featureData->noiseTextureWidth * featureData->noiseTextureWidth);
		std::default_random_engine engine;
		std::uniform_real_distribution<float> u(0.0f, 1.0f);
		for (auto& noise : noiseInfo)
		{
			noise = u(engine);
		}
		featureData->noiseStagingBuffer->WriteData(noiseInfo.data(), sizeof(float) * NOISE_COUNT);
	}

	///Material
	{
		featureData->material = new Core::Graphic::Rendering::Material(_gtaoShader);
		featureData->material->SetUniformBuffer("gtaoInfo", featureData->gtaoInfoBuffer);
		featureData->material->SetSlotData("noiseTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->noiseTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
		featureData->material->SetSlotData("depthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->depthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
	}
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GTAO_Occlusion_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->occlusionTexture;
	delete featureData->gtaoInfoBuffer;
	delete featureData->noiseStagingBuffer;
	delete featureData->noiseTexture;

	delete featureData->material;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<GTAO_Occlusion_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Change layout
	{
		auto depthTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			featureData->depthTexture,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		commandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &depthTextureBarrier }
		);
	}

	///Copy noise to noise texture
	if (featureData->noiseStagingBuffer != nullptr)
	{
		{
			auto noiseTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->noiseTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &noiseTextureBarrier }
			);
		}

		commandBuffer->CopyBufferToImage(featureData->noiseStagingBuffer, featureData->noiseTexture, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		{
			auto noiseTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->noiseTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &noiseTextureBarrier }
			);
		}
	}
	else
	{
		delete featureData->noiseStagingBuffer;
		featureData->noiseStagingBuffer = nullptr;
	}

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);

		featureData->material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		commandBuffer->DrawMesh(_fullScreenMesh, featureData->material);

		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::GTAO_Occlusion_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
