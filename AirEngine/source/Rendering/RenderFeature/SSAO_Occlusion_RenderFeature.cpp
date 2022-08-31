#include "Rendering/RenderFeature/SSAO_Occlusion_RenderFeature.h"
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
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderPass>("AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData>("AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature>("AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderPass::SSAO_Occlusion_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderPass::~SSAO_Occlusion_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData::SSAO_Occlusion_RenderFeatureData()
	: RenderFeatureDataBase()
	, material(nullptr)
	, frameBuffer(nullptr)
	, occlusionTexture(nullptr)
	, occlusionTextureSizeInfoBuffer(nullptr)
	, samplePointInfoBuffer(nullptr)
	, occlusionTextureSize({0, 0})
	, samplePointRadius(1.0f)
	, samplePointBiasAngle(25.0f)
	, depthTexture(nullptr)
	, normalTexture(nullptr)
{

}

AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData::~SSAO_Occlusion_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<SSAO_Occlusion_RenderPass>())
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
	, _ssaoShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\SSAO_Occlusion_Shader.shader"))
	, _textureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
	, _noiseInfoBuffer(nullptr)
{
	_noiseInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(float) * NOISE_COUNT,
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	std::array<float, NOISE_COUNT> noiseInfo = std::array<float, NOISE_COUNT>();
	std::default_random_engine engine;
	std::uniform_real_distribution<float> u(0.0f, 1.0f);
	for (auto& noise : noiseInfo)
	{
		noise = u(engine);
	}
	_noiseInfoBuffer->WriteData(noiseInfo.data(), sizeof(float) * NOISE_COUNT);
}

AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::~SSAO_Occlusion_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<SSAO_Occlusion_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\SSAO_Occlusion_Shader.shader");
	delete _textureSampler;
	delete _noiseInfoBuffer;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new SSAO_Occlusion_RenderFeatureData();
	
	///Occlusion texture
	{
		featureData->occlusionTextureSize = { camera->attachments["ColorAttachment"]->VkExtent2D_().width / 2, camera->attachments["ColorAttachment"]->VkExtent2D_().height / 2 };
		featureData->occlusionTexture = Core::Graphic::Instance::Image::Create2DImage(
			featureData->occlusionTextureSize,
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

	///Occlusion texture size info
	{
		featureData->occlusionTextureSizeInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(AttachmentSizeInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		auto occlusionTextureSize = AttachmentSizeInfo{ glm::vec2(featureData->occlusionTextureSize.width, featureData->occlusionTextureSize.height) , glm::vec2(1, 1) / glm::vec2(featureData->occlusionTextureSize.width, featureData->occlusionTextureSize.height) };
		featureData->occlusionTextureSizeInfoBuffer->WriteData(&occlusionTextureSize, sizeof(AttachmentSizeInfo));
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<SSAO_Occlusion_RenderFeatureData*>(renderFeatureData);

	///Sample point info
	{
		featureData->samplePointInfoBuffer = new Core::Graphic::Instance::Buffer{
			sizeof(glm::vec4) * SAMPLE_POINT_COUNT,
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		Utils::RandomSphericalCoordinateGenerator generator = Utils::RandomSphericalCoordinateGenerator(0, 90 - featureData->samplePointBiasAngle, 0, 360, 1);
		std::array<glm::vec4, SAMPLE_POINT_COUNT> samplePointInfo = std::array<glm::vec4, SAMPLE_POINT_COUNT>();
		for (int i = 0; i < SAMPLE_POINT_COUNT; i++)
		{
			samplePointInfo[i] = glm::vec4(generator.Get() * std::clamp(std::pow(static_cast<float>(i + 1) / static_cast<float>(SAMPLE_POINT_COUNT), 2.0f), 0.1f, 1.0f), featureData->samplePointRadius);
		}
		featureData->samplePointInfoBuffer->WriteData(samplePointInfo.data(), sizeof(glm::vec4) * SAMPLE_POINT_COUNT);
	}

	///Material
	{
		featureData->material = new Core::Graphic::Rendering::Material(_ssaoShader);
		featureData->material->SetUniformBuffer("occlusionTextureSizeInfo", featureData->occlusionTextureSizeInfoBuffer);
		featureData->material->SetUniformBuffer("samplePointInfo", featureData->samplePointInfoBuffer);
		featureData->material->SetUniformBuffer("noiseInfo", _noiseInfoBuffer);
		featureData->material->SetSlotData("depthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->depthTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
		featureData->material->SetSlotData("normalTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _textureSampler->VkSampler_(), featureData->normalTexture->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
	}
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<SSAO_Occlusion_RenderFeatureData*>(renderFeatureData);
	delete featureData->frameBuffer;
	delete featureData->occlusionTexture;
	delete featureData->occlusionTextureSizeInfoBuffer;
	delete featureData->samplePointInfoBuffer;

	delete featureData->material;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<SSAO_Occlusion_RenderFeatureData*>(renderFeatureData);

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
		auto normalTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			featureData->normalTexture,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		commandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			{ &depthTextureBarrier, &normalTextureBarrier }
		);
	}

	///Render
	{
		commandBuffer->BeginRenderPass(
			_renderPass,
			featureData->frameBuffer
		);

		featureData->material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		commandBuffer->DrawMesh(_fullScreenMesh, featureData->material);

		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::SSAO_Occlusion_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
