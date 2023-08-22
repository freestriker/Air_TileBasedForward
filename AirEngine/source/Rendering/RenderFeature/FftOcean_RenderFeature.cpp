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
	, gaussianNoiseTextureStagingBuffer(nullptr)
	, gaussianNoiseTexture(nullptr)
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
	featureData->gaussianNoiseTextureStagingBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(float) * featureData->imageSize.x * featureData->imageSize.y * 2,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	const VkExtent2D imageExtent = VkExtent2D{ uint32_t(featureData->imageSize.x), uint32_t(featureData->imageSize.y) };
	featureData->gaussianNoiseTexture = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32G32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);

	return featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);
	delete featureData->gaussianNoiseTextureStagingBuffer;
	delete featureData->gaussianNoiseTexture;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto& featureData = *static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (featureData.isInitialized)
	{
		delete featureData.gaussianNoiseTextureStagingBuffer;
		featureData.gaussianNoiseTextureStagingBuffer = nullptr;
	}
	else
	{
		//featureData.isInitialized = true;

		{
			std::random_device rd{};
			std::mt19937 gen{ rd() };

			std::uniform_real_distribution<float> normalDistribution(0, 1);
			std::vector<float> dataVector(featureData.imageSize.x * featureData.imageSize.y * 2, 0.0);
			for (int index = 0; index < featureData.imageSize.x * featureData.imageSize.y; ++index)
			{
				constexpr double PI = 3.141592653589793;
				auto&& x1 = normalDistribution(gen);
				auto&& x2 = normalDistribution(gen);
				float g1 = std::sqrt(-2.0f * std::log(x1)) * std::cos(2.0f * PI * x2);
				float g2 = std::sqrt(-2.0f * std::log(x1)) * std::sin(2.0f * PI * x2);
				dataVector[index * 2] = g1;
				dataVector[index * 2 + 1] = g2;
			}

			featureData.gaussianNoiseTextureStagingBuffer->WriteData([&](void* dataPtr)->void {
				memcpy(dataPtr, dataVector.data(), dataVector.size() * sizeof(float));
			});

			{
				auto sourceTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.gaussianNoiseTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &sourceTextureBarrier }
				);
			}

			commandBuffer->CopyBufferToImage(featureData.gaussianNoiseTextureStagingBuffer, featureData.gaussianNoiseTexture, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			{
				auto sourceTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.gaussianNoiseTexture,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_NONE
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					{ &sourceTextureBarrier }
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
