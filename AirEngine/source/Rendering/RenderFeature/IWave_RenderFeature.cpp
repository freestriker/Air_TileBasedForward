#include "Rendering/RenderFeature/IWave_RenderFeature.h"
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

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderPass>("AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderFeatureData>("AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::IWave_RenderFeature>("AirEngine::Rendering::RenderFeature::IWave_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderPass::IWave_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderPass::~IWave_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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

AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderFeatureData::IWave_RenderFeatureData()
	: RenderFeatureDataBase()
	, sourceTexture(nullptr)
	, obstructionTexture(nullptr)
	, kernalImage(nullptr)
	, heightImage(nullptr)
	, previousHeightImage(nullptr)
	, verticalDerivativeImage(nullptr)
{

}

AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderFeatureData::~IWave_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::IWave_RenderFeature::IWave_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<IWave_RenderPass>())
	, _renderPassName(rttr::type::get<IWave_RenderPass>().get_name().to_string())
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

AirEngine::Rendering::RenderFeature::IWave_RenderFeature::~IWave_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<IWave_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new IWave_RenderFeatureData();

	featureData->isInitialized = false;

	featureData->iWaveSurfaceConstantInfo.heightFactor = 1;
	featureData->iWaveSurfaceConstantInfo.minVertexPosition = { -1, -1 };
	featureData->iWaveSurfaceConstantInfo.maxVertexPosition = { 1, 1 };

	featureData->iWaveConstantInfo.imageSize = { 512, 512 };
	featureData->iWaveConstantInfo.halfKernalSize = 6;
	featureData->iWaveConstantInfo.deltaTime = 0;
	featureData->iWaveConstantInfo.alpha = 0.1;
	featureData->iWaveConstantInfo.sourceFactor = 1;
	featureData->iWaveConstantInfo.obstructionFactor = 1;

	VkExtent2D imageExtent = VkExtent2D{ uint32_t(featureData->iWaveConstantInfo.imageSize.x), uint32_t(featureData->iWaveConstantInfo.imageSize.y) };
	VkExtent2D kernalExtent = VkExtent2D{ uint32_t(featureData->iWaveConstantInfo.halfKernalSize * 2 + 1), uint32_t(featureData->iWaveConstantInfo.halfKernalSize * 2 + 1) };
	
	featureData->stagingBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(float) * featureData->iWaveConstantInfo.imageSize.x * featureData->iWaveConstantInfo.imageSize.y + 
		sizeof(float) * featureData->iWaveConstantInfo.imageSize.x * featureData->iWaveConstantInfo.imageSize.y + 
		sizeof(float) * kernalExtent.width * kernalExtent.height,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	featureData->sourceTexture = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->obstructionTexture = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->kernalImage = Core::Graphic::Instance::Image::Create2DImage(
		kernalExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->heightImage = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->previousHeightImage = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->verticalDerivativeImage = Core::Graphic::Instance::Image::Create2DImage(
		imageExtent,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->iWaveShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\IWave_Shader.shader");
	featureData->material = new Core::Graphic::Rendering::Material(featureData->iWaveShader);
	featureData->material->SetSampledImage2D("sourceTexture", featureData->sourceTexture, _linearSampler);
	featureData->material->SetSampledImage2D("obstructionTexture", featureData->obstructionTexture, _linearSampler);
	featureData->material->SetStorageImage2D("kernalImage", featureData->kernalImage);
	featureData->material->SetStorageImage2D("heightImage", featureData->heightImage);
	featureData->material->SetStorageImage2D("previousHeightImage", featureData->previousHeightImage);
	featureData->material->SetStorageImage2D("verticalDerivativeImage", featureData->verticalDerivativeImage);
	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);
	return featureData;
}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<IWave_RenderFeatureData*>(renderFeatureData);
	delete featureData->sourceTexture;
	delete featureData->obstructionTexture;
	delete featureData->kernalImage;
	delete featureData->heightImage;
	delete featureData->previousHeightImage;
	delete featureData->verticalDerivativeImage;
	delete featureData->material;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\IWave_Shader.shader");
	delete featureData->frameBuffer;
	delete featureData;
}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<IWave_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (featureData->isInitialized)
	{
		delete featureData->stagingBuffer;
		featureData->stagingBuffer = nullptr;

		{
			auto heightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->heightImage,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &heightImageBarrier }
			);
		}
	}
	else
	{
		featureData->isInitialized = true;

		std::vector<float> sourceTextureData(featureData->iWaveConstantInfo.imageSize.x * featureData->iWaveConstantInfo.imageSize.y, 0.0);
		{
			auto&& center = featureData->iWaveConstantInfo.imageSize / 2;
			constexpr int SOURCE_HALF_SIZE = 6;
			constexpr float SOURCE_RADIUS = SOURCE_HALF_SIZE * 1.415;
			for (int i = -SOURCE_HALF_SIZE; i <= SOURCE_HALF_SIZE; i++)
			{
				for (int j = -SOURCE_HALF_SIZE; j <= SOURCE_HALF_SIZE; j++)
				{
					auto&& delta = glm::ivec2(i, j);
					auto&& target = center + delta;
					auto&& index = target.x + target.y * featureData->iWaveConstantInfo.imageSize.x;
					sourceTextureData[index] = ((SOURCE_RADIUS - glm::length(glm::vec2(delta))) / SOURCE_RADIUS) * 0.1;
				}
			}
		}
		std::vector<float> obstructionTextureData(featureData->iWaveConstantInfo.imageSize.x * featureData->iWaveConstantInfo.imageSize.y, 1.0);
		{

		}
		std::vector<float> kernalImageData((featureData->iWaveConstantInfo.halfKernalSize * 2 + 1) * (featureData->iWaveConstantInfo.halfKernalSize * 2 + 1), 0.0);
		{
			double dk = 0.01;
			double sigma = 1.0;
			double norm = 0;
			for (double k = 0; k < 10; k += dk)
			{
				norm += k * k * std::exp(-sigma * k * k);
			}
			for (int i = -featureData->iWaveConstantInfo.halfKernalSize; i <= featureData->iWaveConstantInfo.halfKernalSize; i++)
			{
				for (int j = -featureData->iWaveConstantInfo.halfKernalSize; j <= featureData->iWaveConstantInfo.halfKernalSize; j++)
				{
					double r = std::sqrt((float)(i * i + j * j));
					double kern = 0;
					for (double k = 0; k < 10; k += dk)
					{
						kern += k * k * exp(-sigma * k * k) * j0(r * k);
					}
					auto&& index = j + featureData->iWaveConstantInfo.halfKernalSize + (i + featureData->iWaveConstantInfo.halfKernalSize) * (2 * featureData->iWaveConstantInfo.halfKernalSize + 1);
					kernalImageData[index] = kern / norm;
				}
			}
		}
		{
			featureData->stagingBuffer->WriteData([&](void* dataPtr)->void {
				float* data = reinterpret_cast<float*>(dataPtr);
				int floatCount = 0;
				memcpy(data + floatCount, sourceTextureData.data(), sourceTextureData.size() * sizeof(float));
				floatCount += sourceTextureData.size();
				memcpy(data + floatCount, obstructionTextureData.data(), obstructionTextureData.size() * sizeof(float));
				floatCount += obstructionTextureData.size();
				memcpy(data + floatCount, kernalImageData.data(), kernalImageData.size() * sizeof(float));
			});
		}
		{
			auto sourceTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->sourceTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			auto obstructionTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->obstructionTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			auto kernalImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->kernalImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			auto heightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->heightImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			auto previousHeightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->previousHeightImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			auto verticalDerivativeImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->verticalDerivativeImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &sourceTextureBarrier , &obstructionTextureBarrier , &kernalImageBarrier , &heightImageBarrier , &previousHeightImageBarrier, &verticalDerivativeImageBarrier }
			);
		}
		commandBuffer->CopyBufferToImage(featureData->stagingBuffer, 0, featureData->sourceTexture, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		commandBuffer->CopyBufferToImage(featureData->stagingBuffer, sourceTextureData.size() * sizeof(float), featureData->obstructionTexture, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		commandBuffer->CopyBufferToImage(featureData->stagingBuffer, (sourceTextureData.size() + obstructionTextureData.size()) * sizeof(float), featureData->kernalImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VkClearColorValue clearColorValue;
		clearColorValue.float32[0] = 0; clearColorValue.float32[1] = 0; clearColorValue.float32[2] = 0; clearColorValue.float32[3] = 0;
		commandBuffer->ClearColorImage(featureData->heightImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearColorValue);
		commandBuffer->ClearColorImage(featureData->previousHeightImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearColorValue);
		commandBuffer->ClearColorImage(featureData->verticalDerivativeImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearColorValue);

		{
			auto sourceTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->sourceTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			auto obstructionTextureBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->obstructionTexture,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			auto kernalImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->kernalImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto heightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->heightImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto previousHeightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->previousHeightImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto verticalDerivativeImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->verticalDerivativeImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &sourceTextureBarrier , &obstructionTextureBarrier , &kernalImageBarrier , &heightImageBarrier , &previousHeightImageBarrier, &verticalDerivativeImageBarrier }
			);
		}
	}

	///Wait light lists buffer ready
	{
		auto&& deltaTime = Core::Logic::CoreObject::Instance::time.DeltaDuration();
		featureData->iWaveConstantInfo.deltaTime = 0.03/*Core::Logic::CoreObject::Instance::time.DeltaDuration() > 0.03 ? 0.003 : Core::Logic::CoreObject::Instance::time.DeltaDuration()*/;
		commandBuffer->PushConstant(featureData->material, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, featureData->iWaveConstantInfo);
		constexpr int LOCAL_GROUP_WIDTH = 8;
		commandBuffer->Dispatch(featureData->material, (featureData->iWaveConstantInfo.imageSize.x + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, (featureData->iWaveConstantInfo.imageSize.y + LOCAL_GROUP_WIDTH - 1) / LOCAL_GROUP_WIDTH, 1);
		featureData->iWaveConstantInfo.sourceFactor = 0;
	}

	{
		auto heightImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
		(
			featureData->heightImage,
			VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
		);
		commandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			{ &heightImageBarrier }
		);
	}

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);

		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_renderPassName);
			if (material == nullptr) continue;

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());
			material->SetSampledImage2D("heightTexture", featureData->heightImage, _linearSampler);

			commandBuffer->PushConstant(material, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, featureData->iWaveSurfaceConstantInfo);
			commandBuffer->DrawMesh(rendererComponent->mesh, material);
		}
		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::IWave_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
