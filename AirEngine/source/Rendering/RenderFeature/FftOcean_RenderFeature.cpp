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
#include <qlineedit.h>
#include <QIntValidator>
#include <QDoubleValidator>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_Surface_RenderPass>("AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_Surface_RenderPass")
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
AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_Surface_RenderPass::FftOcean_Surface_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_Surface_RenderPass::~FftOcean_Surface_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_Surface_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<FftOcean_Surface_RenderPass>())
	, _renderPassName(rttr::type::get<FftOcean_Surface_RenderPass>().get_name().to_string())
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
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<FftOcean_Surface_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new FftOcean_RenderFeatureData();

	featureData->isInitialized = false;
	featureData->imageSize = { 512, 512 };
	featureData->L = 512;
	featureData->NM = featureData->imageSize;
	featureData->windRotationAngle = 0;
	featureData->windSpeed = 31;
	featureData->a = 3;
	featureData->windDependency = 0.1;
	featureData->displacementFactor = { 1, 1, 1 };
	featureData->minVertexPosition = { 0, 0 };
	featureData->maxVertexPosition = { 1, 1 };
	featureData->displacementFactor = { 1, 1, 1 };
	featureData->normalScale = 1;
	featureData->bubblesLambda = 1;
	featureData->bubblesThreshold = 1;
	featureData->bubblesScale = 85;

	featureData->launcher = new FftOceanDataWindowLauncher(*featureData);
	featureData->launcher->moveToThread(QApplication::instance()->thread());
	QApplication::postEvent(featureData->launcher, new QEvent(QEvent::User));

	const VkExtent2D imageExtent = VkExtent2D{ uint32_t(featureData->imageSize.x), uint32_t(featureData->imageSize.y) };

	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);

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
	}

	{
		featureData->imageArray = Core::Graphic::Instance::Image::Create2DImageArray(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			7
		);
		for (int layerIndex = 0; layerIndex < featureData->imageArray->LayerCount(); ++layerIndex)
		{
			featureData->imageArray->AddImageView(
				"ImageView" + std::to_string(layerIndex),
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
				VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
				layerIndex, 1
			);
		}
		featureData->imageArray->AddImageView(
			"SpectrumImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			0, 3
		);
		featureData->imageArray->AddImageView(
			"ImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			4, 3
		);
	}

	{
		featureData->spectrumShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Spectrum_Shader.shader");
		featureData->spectrumMaterial = new Core::Graphic::Rendering::Material(featureData->spectrumShader);
		featureData->spectrumMaterial->SetStorageImage2D("phillipsSpectrumImage", featureData->phillipsSpectrumImage);
		featureData->spectrumMaterial->SetStorageImage2D("imageArray", featureData->imageArray, "SpectrumImageGroup");
	}

	{
		featureData->ifftShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Ifft_Shader.shader");
		featureData->ifftMaterial = new Core::Graphic::Rendering::Material(featureData->ifftShader);
		featureData->ifftMaterial->SetStorageImage2D("imageArray", featureData->imageArray);
	}

	{
		featureData->displacementImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->normalImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		
		featureData->resolveShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Resolve_Shader.shader");
		featureData->resolveMaterial = new Core::Graphic::Rendering::Material(featureData->resolveShader);
		featureData->resolveMaterial->SetStorageImage2D("imageArray", featureData->imageArray);
		featureData->resolveMaterial->SetStorageImage2D("displacementImage", featureData->displacementImage);
		featureData->resolveMaterial->SetStorageImage2D("normalImage", featureData->normalImage);
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	//QApplication::postEvent(featureData->launcher->window, new QEvent(QEvent::Close));
	//QApplication::postEvent(featureData->launcher->window, new QEvent(QEvent::Destroy));
	featureData->launcher->deleteLater();
	//QApplication::postEvent(featureData->launcher, new QEvent(QEvent::Destroy));
	//delete featureData->launcher;

	delete featureData->frameBuffer;

	delete featureData->gaussianNoiseImage;
	delete featureData->gaussianNoiseImageStagingBuffer;

	delete featureData->imageArray;

	delete featureData->phillipsSpectrumImage;
	delete featureData->phillipsSpectrumMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_PhillipsSpectrum_Shader.shader");

	delete featureData->spectrumMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Spectrum_Shader.shader");

	delete featureData->ifftMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Ifft_Shader.shader");

	delete featureData->displacementImage;
	delete featureData->normalImage;
	delete featureData->resolveMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Resolve_Shader.shader");

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

	// phillips spectrum
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
		struct PhillipsSpectrumInfo
		{
			glm::ivec2 imageSize;
			glm::ivec2 NM;
			glm::vec2 windDirection;
			float L;
			float windSpeed;
			float time;
			float a;
			float windDependency;
		};
		PhillipsSpectrumInfo phillipsSpectrumInfo{};
		phillipsSpectrumInfo.imageSize = featureData.imageSize;
		phillipsSpectrumInfo.NM = featureData.NM;
		constexpr double PI = 3.141592653589793;
		const auto&& radian = featureData.windRotationAngle / 360 * 2 * PI;
		phillipsSpectrumInfo.windDirection = glm::normalize(glm::vec2(std::cos(radian), std::sin(radian)));
		phillipsSpectrumInfo.L = featureData.L;
		phillipsSpectrumInfo.windSpeed = featureData.windSpeed;
		phillipsSpectrumInfo.time = time;
		phillipsSpectrumInfo.a = featureData.a;
		phillipsSpectrumInfo.windDependency = featureData.windDependency;

		commandBuffer->PushConstant(featureData.phillipsSpectrumMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, phillipsSpectrumInfo);
		commandBuffer->Dispatch(featureData.phillipsSpectrumMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);

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

	int heightSpectrumImageIndex = 0;
	int xSpectrumImageIndex = 1;
	int ySpectrumImageIndex = 2;
	int tempImageIndex = 3;
	int heightImageIndex = 4;
	int xImageIndex = 5;
	int yImageIndex = 6;

	// spectrum
	{
		{
			auto phillipsSpectrumImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.phillipsSpectrumImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			auto tempImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"SpectrumImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &phillipsSpectrumImageBarrier, &tempImageBarrier }
			);
		}

		struct SpectrumInfo
		{
			glm::ivec2 imageSize;
			glm::ivec2 NM;
			float L;
			float time;
			int heightSpectrumImageIndex;
			int xSpectrumImageIndex;
			int ySpectrumImageIndex;
			int xSlopeSpectrumImageIndex;
			int ySlopeSpectrumImageIndex;
		};
		SpectrumInfo spectrumInfo{};
		spectrumInfo.imageSize = featureData.imageSize;
		spectrumInfo.NM = featureData.NM;
		spectrumInfo.L = featureData.L;
		spectrumInfo.time = time;
		spectrumInfo.heightSpectrumImageIndex = heightSpectrumImageIndex;
		spectrumInfo.xSpectrumImageIndex = xSpectrumImageIndex;
		spectrumInfo.ySpectrumImageIndex = ySpectrumImageIndex;

		commandBuffer->PushConstant(featureData.spectrumMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, spectrumInfo);
		commandBuffer->Dispatch(featureData.spectrumMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
	}

	// copy
	{
		{
			auto spectrumImageGroupBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"SpectrumImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT
			);
			auto imageGroupBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"ImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &spectrumImageGroupBarrier, &imageGroupBarrier }
			);
		}

		commandBuffer->CopyImage(
			featureData.imageArray, "SpectrumImageGroup", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			featureData.imageArray, "ImageGroup", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
	}

	// ifft
	{
		{
			auto spectrumImageGroupBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"SpectrumImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto tempImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"ImageView" + std::to_string(tempImageIndex),
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto imageGroupBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"ImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &spectrumImageGroupBarrier, &tempImageBarrier, &imageGroupBarrier }
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
		const int ifftXCount = std::log2(featureData.NM.x);
		const int ifftYCount = std::log2(featureData.NM.y);

		IfftConstantInfo ifftConstantInfo{};
		ifftConstantInfo.imageSize = featureData.imageSize;
		ifftConstantInfo.NM = featureData.NM;

		// height
		{
			auto& sourceImageIndex = heightImageIndex;
			auto& targetImageIndex = tempImageIndex;

			ifftConstantInfo.isHorizen = 1;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftXCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftXCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}

			ifftConstantInfo.isHorizen = 0;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftYCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftYCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}
		}

		// x
		{
			auto& sourceImageIndex = xImageIndex;
			auto& targetImageIndex = tempImageIndex;

			ifftConstantInfo.isHorizen = 1;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftXCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftXCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}

			ifftConstantInfo.isHorizen = 0;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftYCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftYCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}
		}

		// y
		{
			auto& sourceImageIndex = yImageIndex;
			auto& targetImageIndex = tempImageIndex;

			ifftConstantInfo.isHorizen = 1;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftXCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftXCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}

			ifftConstantInfo.isHorizen = 0;
			for (int ifftIndex = 0, blockSize = 1; ifftIndex < ifftYCount; ++ifftIndex, blockSize *= 2)
			{
				ifftConstantInfo.isLast = (ifftIndex == ifftYCount - 1) ? 1 : 0;
				ifftConstantInfo.blockSize = blockSize;
				ifftConstantInfo.sourceIndex = sourceImageIndex;
				ifftConstantInfo.targetIndex = targetImageIndex;

				commandBuffer->PushConstant(featureData.ifftMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, ifftConstantInfo);
				commandBuffer->Dispatch(featureData.ifftMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
				{
					auto imageArrayBarrier = Core::Graphic::Command::ImageMemoryBarrier
					(
						featureData.imageArray,
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex),
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
						VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
						VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
					);
					commandBuffer->AddPipelineImageBarrier(
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						{ &imageArrayBarrier }
					);
				}

				std::swap(sourceImageIndex, targetImageIndex);
			}
		}
	}

	// resolve
	{
		struct ResolveConstantInfo
		{
			glm::ivec2 imageSize;
			glm::ivec2 NM;
			int heightImageIndex;
			int xImageIndex;
			int yImageIndex;
			//int xSlopeImageIndex;
			//int ySlopeImageIndex;
			alignas(8) glm::vec3 displacementFactor;
			//alignas(8) glm::vec2 L;
			alignas(8) glm::ivec2 meshEdgeVertexCount;
			float normalScale;
			float bubblesLambda;
			float bubblesThreshold;
			float bubblesScale;
		};
		ResolveConstantInfo resolveConstantInfo{};
		resolveConstantInfo.imageSize = featureData.imageSize;
		resolveConstantInfo.NM = featureData.NM;
		resolveConstantInfo.heightImageIndex = heightImageIndex;
		resolveConstantInfo.xImageIndex = xImageIndex;
		resolveConstantInfo.yImageIndex = yImageIndex;
		resolveConstantInfo.displacementFactor = featureData.displacementFactor;
		//resolveConstantInfo.xSlopeImageIndex = xSlopeImageIndex;
		//resolveConstantInfo.ySlopeImageIndex = ySlopeImageIndex;
		resolveConstantInfo.meshEdgeVertexCount = { 257, 257 };
		//resolveConstantInfo.L = { featureData.L, featureData.L };
		resolveConstantInfo.normalScale = featureData.normalScale;
		resolveConstantInfo.bubblesLambda = featureData.bubblesLambda;
		resolveConstantInfo.bubblesThreshold = featureData.bubblesThreshold;
		resolveConstantInfo.bubblesScale = featureData.bubblesScale;

		{
			auto displacementImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.displacementImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto normalImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.normalImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &displacementImageBarrier, &normalImageBarrier }
			);
		}

		commandBuffer->PushConstant(featureData.resolveMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, resolveConstantInfo);
		commandBuffer->Dispatch(featureData.resolveMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
		
		{
			auto displacementImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.displacementImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			auto normalImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.normalImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
				{ &displacementImageBarrier, &normalImageBarrier }
			);
		}
}

	///Render
	{
		commandBuffer->BeginRenderPass(_renderPass, featureData.frameBuffer);

		//struct SurfaceConstantInfo
		//{
		//	glm::vec3 displacementFactor;
		//};
		//SurfaceConstantInfo surfaceConstantInfo{};
		////surfaceConstantInfo.minVertexPosition = featureData.minVertexPosition;
		////surfaceConstantInfo.maxVertexPosition = featureData.maxVertexPosition;
		//surfaceConstantInfo.displacementFactor = featureData.displacementFactor;

		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_renderPassName);
			if (material == nullptr) continue;

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());
			material->SetSampledImage2D("displacementTexture", featureData.displacementImage, _linearSampler);
			material->SetSampledImage2D("normalTexture", featureData.normalImage, _pointSampler);
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());

			//commandBuffer->PushConstant(material, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, surfaceConstantInfo);
			commandBuffer->DrawMesh(rendererComponent->mesh, material);
		}
		commandBuffer->EndRenderPass();
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

//#include "FftOcean_RenderFeature.moc"

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOceanDataWindow::BuildLayout()
{
	auto&& fftOceanDataPtr = &_fftOceanData;

	QFormLayout* pLayout = new QFormLayout(this);

	QIntValidator* intValidator = new QIntValidator;
	intValidator->setRange(0, 10000000);

	QDoubleValidator* doubleValidator = new QDoubleValidator;
	doubleValidator->setRange(0, 10000000, 5);

	// L
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->L)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->L = string.toFloat();
			Utils::Log::Message("L: " + std::to_string(fftOceanDataPtr->L));
		});
		pLayout->addRow(QStringLiteral("L: "), lineEdit);
	}

	// windRotationAngle
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->windRotationAngle)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->windRotationAngle = string.toFloat();
			Utils::Log::Message("windRotationAngle: " + std::to_string(fftOceanDataPtr->windRotationAngle));
		});
		pLayout->addRow(QStringLiteral("windRotationAngle: "), lineEdit);
	}

	// windSpeed
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->windSpeed)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->windSpeed = string.toFloat();
			Utils::Log::Message("windSpeed: " + std::to_string(fftOceanDataPtr->windSpeed));
		});
		pLayout->addRow(QStringLiteral("windSpeed: "), lineEdit);
	}

	// a
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->a)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->a = string.toFloat();
			Utils::Log::Message("a: " + std::to_string(fftOceanDataPtr->a));
		});
		pLayout->addRow(QStringLiteral("a: "), lineEdit);
	}

	// windDependency
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->windDependency)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->windDependency = string.toFloat();
			Utils::Log::Message("windDependency: " + std::to_string(fftOceanDataPtr->windDependency));
		});
		pLayout->addRow(QStringLiteral("windDependency: "), lineEdit);
	}

	// displacement.height
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->displacementFactor.y)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->displacementFactor.y = string.toFloat();
			Utils::Log::Message("displacement.height: " + std::to_string(fftOceanDataPtr->displacementFactor.y));
		});
		pLayout->addRow(QStringLiteral("displacement.height: "), lineEdit);
	}

	// displacementFactor.x
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->displacementFactor.x)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->displacementFactor.x = string.toFloat();
			Utils::Log::Message("displacementFactor.x: " + std::to_string(fftOceanDataPtr->displacementFactor.x));
		});
		pLayout->addRow(QStringLiteral("displacementFactor.x: "), lineEdit);
	}

	// displacementFactor.z
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->displacementFactor.z)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->displacementFactor.z = string.toFloat();
			Utils::Log::Message("displacementFactor.z: " + std::to_string(fftOceanDataPtr->displacementFactor.z));
		});
		pLayout->addRow(QStringLiteral("displacementFactor.z: "), lineEdit);
	}

	// normalScale
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->normalScale)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->normalScale = string.toFloat();
			Utils::Log::Message("normalScale: " + std::to_string(fftOceanDataPtr->normalScale));
		});
		pLayout->addRow(QStringLiteral("normalScale: "), lineEdit);
	}

	// bubblesLambda
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->bubblesLambda)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->bubblesLambda = string.toFloat();
			Utils::Log::Message("bubblesLambda: " + std::to_string(fftOceanDataPtr->bubblesLambda));
		});
		pLayout->addRow(QStringLiteral("bubblesLambda: "), lineEdit);
	}

	// bubblesThreshold
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->bubblesThreshold)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->bubblesThreshold = string.toFloat();
			Utils::Log::Message("bubblesThreshold: " + std::to_string(fftOceanDataPtr->bubblesThreshold));
		});
		pLayout->addRow(QStringLiteral("bubblesThreshold: "), lineEdit);
	}

	// bubblesScale
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->bubblesScale)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->bubblesScale = string.toFloat();
			Utils::Log::Message("bubblesScale: " + std::to_string(fftOceanDataPtr->bubblesScale));
		});
		pLayout->addRow(QStringLiteral("bubblesScale: "), lineEdit);
	}
	setLayout(pLayout);
}
