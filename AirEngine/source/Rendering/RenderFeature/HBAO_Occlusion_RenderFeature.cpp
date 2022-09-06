#include "Rendering/RenderFeature/HBAO_Occlusion_RenderFeature.h"
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
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderPass>("AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeatureData>("AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature>("AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderPass::HBAO_Occlusion_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderPass::~HBAO_Occlusion_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
		{ "OcclusionTexture"}
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0
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

AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeatureData::HBAO_Occlusion_RenderFeatureData()
	: RenderFeatureDataBase()
	, material(nullptr)
	, frameBuffer(nullptr)
	, occlusionTexture(nullptr)
	, hbaoInfoBuffer(nullptr)
	, noiseTexture(nullptr)
	, noiseStagingBuffer(nullptr)
	, sampleRadius(0.2f)
	, sampleBiasAngle(35.0f)
	, stepCount(4)
	, directionCount(8)
	, noiseTextureWidth(64)
	, depthTexture(nullptr)
{

}

AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeatureData::~HBAO_Occlusion_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<HBAO_Occlusion_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _hbaoShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\HBAO_Occlusion_Shader.shader"))
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

AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::~HBAO_Occlusion_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<HBAO_Occlusion_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\HBAO_Occlusion_Shader.shader");
	delete _textureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new HBAO_Occlusion_RenderFeatureData();
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

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<HBAO_Occlusion_RenderFeatureData*>(renderFeatureData);
	VkExtent2D extent = { camera->attachments["ColorAttachment"]->VkExtent2D_().width / 2, camera->attachments["ColorAttachment"]->VkExtent2D_().height / 2 };

	///Occlusion texture size info
	{
		featureData->hbaoInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(HbaoInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		featureData->hbaoInfoBuffer->WriteData(
			[featureData, extent](void* ptr)->void
			{
				HbaoInfo* hbaoInfo = reinterpret_cast<HbaoInfo*>(ptr);
				hbaoInfo->attachmentize = glm::vec2(extent.width, extent.height);
				hbaoInfo->attachmentTexelSize = glm::vec2(1, 1) / hbaoInfo->attachmentize;
				hbaoInfo->sampleRadius = featureData->sampleRadius;
				hbaoInfo->sampleBiasAngle = featureData->sampleBiasAngle;
				hbaoInfo->stepCount = featureData->stepCount;
				hbaoInfo->directionCount = featureData->directionCount;
				hbaoInfo->noiseTextureWidth = featureData->noiseTextureWidth;
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
		featureData->material = new Core::Graphic::Rendering::Material(_hbaoShader);
		featureData->material->SetUniformBuffer("hbaoInfo", featureData->hbaoInfoBuffer);
		featureData->material->SetSlotData("depthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->depthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
	}
}

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<HBAO_Occlusion_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->occlusionTexture;
	delete featureData->hbaoInfoBuffer;
	delete featureData->noiseStagingBuffer;
	delete featureData->noiseTexture;

	delete featureData->material;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<HBAO_Occlusion_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Copy noise to noise texture
	if (featureData->noiseTexture == nullptr)
	{
		featureData->noiseTexture = Core::Graphic::Instance::Image::Create2DImage(
			{ static_cast<uint32_t>(featureData->noiseTextureWidth), static_cast<uint32_t>(featureData->noiseTextureWidth) },
			VK_FORMAT_R32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->material->SetSlotData("noiseTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->noiseTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });

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

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::HBAO_Occlusion_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
