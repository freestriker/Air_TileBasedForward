#include "Rendering/RenderFeature/FftOcean_RenderFeature.h"
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
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Rendering/RenderFeature/CSM_ShadowCaster_RenderFeature.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Instance/Memory.h"
#include <cmath>
#include <corecrt_math.h>
#include <Core/Logic/CoreObject/Instance.h>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include <random>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderPass>("AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeatureData>("AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature>("AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderPass::FftOcean_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderPass::~FftOcean_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	settings.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" },
		"DepthAttachment"
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeatureData::FftOcean_RenderFeatureData()
	: RenderFeatureDataBase()
	, isInitialized(false)
	, gaussianNoiseImageStagingBuffer(nullptr)
	, gaussianNoiseImage(nullptr)
{

}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeatureData::~FftOcean_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<FftOcean_RenderPass>())
	, _renderPassName(rttr::type::get<FftOcean_RenderPass>().get_name().to_string())
	, _pointSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
	, _linearSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_LINEAR,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{
	_description = "Use forward+ to render opaque objects, mixing shadows with ambient occlusion.";
}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::~FftOcean_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<FftOcean_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new FftOcean_RenderFeatureData();

	featureData->isInitialized = false;
	featureData->imageSize = { 512, 512 };
	const VkExtent2D imageExtent = VkExtent2D{ uint32_t(featureData->imageSize.x), uint32_t(featureData->imageSize.y) };

	{
		featureData->gaussianNoiseImageStagingBuffer = new Core::Graphic::Instance::Buffer(
			sizeof(float) * featureData->imageSize.x * featureData->imageSize.y * 4,
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		featureData->gaussianNoiseImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
	}

	{
		featureData->generateFrequencyInfo.imageSize = featureData->imageSize;
		featureData->generateFrequencyInfo.L = glm::vec2(1000, 1000);
		featureData->generateFrequencyInfo.NM = featureData->imageSize;
		featureData->generateFrequencyInfo.windDirection = glm::normalize(glm::vec2(1, 0.5));
		featureData->generateFrequencyInfo.windSpeed = 31;
		featureData->generateFrequencyInfo.time = 0;
		featureData->generateFrequencyInfo.a = 3;
		featureData->generateFrequencyInfo.windDependency = 0.1;
		
		featureData->phillipsSpectrumShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_PhillipsSpectrum_Shader.shader");
		featureData->phillipsSpectrumMaterial = new Core::Graphic::Rendering::Material(featureData->phillipsSpectrumShader);
		featureData->phillipsSpectrumImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->phillipsSpectrumMaterial->SetStorageImage2D("gaussianNoiseImage", featureData->gaussianNoiseImage);
		featureData->phillipsSpectrumMaterial->SetStorageImage2D("phillipsSpectrumImage", featureData->phillipsSpectrumImage);

		featureData->generateFrequencyShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_GenerateFrequency_Shader.shader");
		featureData->generateFrequencyMaterial = new Core::Graphic::Rendering::Material(featureData->generateFrequencyShader);
		featureData->heightFrequencyImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->xyFrequencyImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->xySlopeFrequencyImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->generateFrequencyMaterial->SetStorageImage2D("phillipsSpectrumImage", featureData->phillipsSpectrumImage);
		featureData->generateFrequencyMaterial->SetStorageImage2D("heightFrequencyImage", featureData->heightFrequencyImage);
		featureData->generateFrequencyMaterial->SetStorageImage2D("xyFrequencyImage", featureData->xyFrequencyImage);
		featureData->generateFrequencyMaterial->SetStorageImage2D("xySlopeFrequencyImage", featureData->xySlopeFrequencyImage);
	}

	{
		featureData->ifftShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Ifft_Shader.shader");
		featureData->ifftMaterial = new Core::Graphic::Rendering::Material(featureData->ifftShader);
		featureData->tempImageArray = Core::Graphic::Instance::Image::Create2DImageArray(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			2
		);
		featureData->tempImageArray->AddImageView(
			"ImageView0",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			0, 1
		);
		featureData->tempImageArray->AddImageView(
			"ImageView1",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			1, 1
		);
		featureData->ifftMaterial->SetStorageImage2D("tempImageArray", featureData->tempImageArray);
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	delete featureData->gaussianNoiseImageStagingBuffer;
	delete featureData->gaussianNoiseImage;

	delete featureData->generateFrequencyMaterial;
	delete featureData->heightFrequencyImage;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_GenerateFrequency_Shader.shader");

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto& featureData = *static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);
	constexpr int LOCAL_GROUP_WIDTH = 16;

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (featureData.isInitialized)
	{
		delete featureData.gaussianNoiseImageStagingBuffer;
		featureData.gaussianNoiseImageStagingBuffer = nullptr;
	}
	else
	{
		featureData.isInitialized = true;

		{
			std::random_device rd{};
			std::mt19937 gen{ rd() };

			std::normal_distribution<float> normalDistribution(0, 1);
			std::vector<float> dataVector(featureData.imageSize.x * featureData.imageSize.y * 4, 0.0);
			for (int index = 0; index < featureData.imageSize.x * featureData.imageSize.y; ++index)
			{
				constexpr double PI = 3.141592653589793;
				auto&& x1 = normalDistribution(gen);
				auto&& x2 = normalDistribution(gen);
				auto&& x3 = normalDistribution(gen);
				auto&& x4 = normalDistribution(gen);
				//float g1 = std::sqrt(-2.0f * std::log(x1)) * std::cos(2.0f * PI * x2);
				//float g2 = std::sqrt(-2.0f * std::log(x1)) * std::sin(2.0f * PI * x2);
				//dataVector[index * 2] = g1;
				//dataVector[index * 2 + 1] = g2;
				dataVector[index * 4] = x1;
				dataVector[index * 4 + 1] = x2;
				dataVector[index * 4 + 2] = x3;
				dataVector[index * 4 + 3] = x4;
			}

			featureData.gaussianNoiseImageStagingBuffer->WriteData([&](void* dataPtr)->void {
				memcpy(dataPtr, dataVector.data(), dataVector.size() * sizeof(float));
			});

			{
				auto gaussianNoiseImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.gaussianNoiseImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &gaussianNoiseImageBarrier }
				);
			}

			commandBuffer->CopyBufferToImage(featureData.gaussianNoiseImageStagingBuffer, featureData.gaussianNoiseImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			{
				auto gaussianNoiseImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.gaussianNoiseImage,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					{ &gaussianNoiseImageBarrier }
				);
			}
		}
	}

	const auto&& time = Core::Logic::CoreObject::Instance::time.LaunchDuration();
	featureData.generateFrequencyInfo.time = time;

	// phillipsSpectrum
	{
		{
			auto phillipsSpectrumImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.phillipsSpectrumImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &phillipsSpectrumImageBarrier }
			);
		}

		commandBuffer->PushConstant(featureData.phillipsSpectrumMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, featureData.generateFrequencyInfo);
		commandBuffer->Dispatch(featureData.phillipsSpectrumMaterial, (featureData.imageSize.x + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, (featureData.imageSize.y + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, 1);

		{
			auto phillipsSpectrumImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.phillipsSpectrumImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &phillipsSpectrumImageBarrier }
			);
		}
	}

	// orthor
	{
		{
			auto heightFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.heightFrequencyImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto xyFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.xyFrequencyImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto xySlopeFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.xySlopeFrequencyImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &heightFrequencyImageBarrier, &xyFrequencyImageBarrier, &xySlopeFrequencyImageBarrier }
			);
		}

		commandBuffer->PushConstant(featureData.generateFrequencyMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, featureData.generateFrequencyInfo);
		commandBuffer->Dispatch(featureData.generateFrequencyMaterial, (featureData.imageSize.x + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, (featureData.imageSize.y + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, 1);
	}

	//{
	//	auto heightFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
	//	(
	//		featureData.heightFrequencyImage,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
	//	);
	//	auto xyFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
	//	(
	//		featureData.xyFrequencyImage,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
	//	);
	//	auto xySlopeFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
	//	(
	//		featureData.xySlopeFrequencyImage,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
	//		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
	//	);
	//	commandBuffer->AddPipelineImageBarrier(
	//		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
	//		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
	//		{ &heightFrequencyImageBarrier, &xyFrequencyImageBarrier, &xySlopeFrequencyImageBarrier, &tempImageBarrier }
	//	);
	//}

	// height ifft
	{
		{
			auto heightFrequencyImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.heightFrequencyImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
			);
			auto tempImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.tempImageArray,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &heightFrequencyImageBarrier, &tempImageBarrier }
			);
		}
		commandBuffer->CopyImage(
			featureData.heightFrequencyImage, "DefaultImageView", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			featureData.tempImageArray, "ImageView0", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
		{
			auto tempImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.tempImageArray,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &tempImageBarrier }
			);
		}

		struct IfftConstantInfo
		{
			glm::ivec2 imageSize;
			glm::ivec2 NM;
			int isLast;
			int isHorizen;
			int blockSize;
			int sourceIndex;
			int targetIndex;
		};
		IfftConstantInfo ifftConstantInfo{};
		ifftConstantInfo.imageSize = featureData.imageSize;
		ifftConstantInfo.NM = featureData.generateFrequencyInfo.NM;
		ifftConstantInfo.isLast = 0;
		ifftConstantInfo.isHorizen = 0;
		ifftConstantInfo.blockSize = 0;
		ifftConstantInfo.sourceIndex = 0;
		ifftConstantInfo.targetIndex = 0;

		const int ifftXCount = std::log2(featureData.generateFrequencyInfo.NM.x);
		for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftXCount; ++ifftIndex, blockSize *= 2)
		{
			ifftConstantInfo.isLast = (ifftIndex == ifftXCount - 1) ? 1 : 0;
			ifftConstantInfo.isHorizen = 1;
			ifftConstantInfo.blockSize = blockSize;
			ifftConstantInfo.sourceIndex = (ifftConstantInfo.targetIndex) % 2;
			ifftConstantInfo.targetIndex = (ifftConstantInfo.sourceIndex + 1) % 2;

			commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
			commandBuffer->Dispatch(featureData.ifftMaterial, (featureData.imageSize.x + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, (featureData.imageSize.y + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, 1);
			{
				auto tempImageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.tempImageArray,
					"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
					VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
					VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
					VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					{ &tempImageArrayBarrier }
				);
			}
		}

		const int ifftYCount = std::log2(featureData.generateFrequencyInfo.NM.y);
		for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftYCount; ++ifftIndex, blockSize *= 2)
		{
			ifftConstantInfo.isLast = (ifftIndex == ifftYCount - 1) ? 1 : 0;
			ifftConstantInfo.isHorizen = 0;
			ifftConstantInfo.blockSize = blockSize;
			ifftConstantInfo.sourceIndex = (ifftConstantInfo.targetIndex) % 2;
			ifftConstantInfo.targetIndex = (ifftConstantInfo.sourceIndex + 1) % 2;

			commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
			commandBuffer->Dispatch(featureData.ifftMaterial, (featureData.imageSize.x + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, (featureData.imageSize.y + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, 1);
			{
				auto tempImageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.tempImageArray,
					"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
					VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
					VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
					VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					{ &tempImageArrayBarrier }
				);
			}
		}
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
