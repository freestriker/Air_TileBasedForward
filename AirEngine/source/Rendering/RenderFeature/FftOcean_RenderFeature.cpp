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
#include <glm/gtx/intersect.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <Camera/PerspectiveCamera.h>
#include <QCheckBox>

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
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
	, _linearSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_LINEAR,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT,
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
	featureData->isDirty = true;
	featureData->imageSize = { 512, 512 };
	featureData->L = 512;
	featureData->windRotationAngle = 0;
	featureData->windSpeed = 31;
	featureData->a = 3;
	featureData->windDependency = 0.1;
	featureData->minVertexPosition = { 0, 0 };
	featureData->maxVertexPosition = { 1, 1 };
	featureData->displacementFactor = { 1, 1, 1 };
	featureData->normalScale = 1;
	featureData->bubblesLambda = 1;
	featureData->bubblesThreshold = 1;
	featureData->bubblesScale = 85;
	featureData->oceanScale = 5;
	featureData->absDisplacement = glm::vec3(0.12, 0.12, 0.12);
	featureData->showWireFrame = false;

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
	}

	{
		featureData->imageArray = Core::Graphic::Instance::Image::Create2DImageArray(
			imageExtent,
			VkFormat::VK_FORMAT_R32G32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			11
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
			"GaussianNoiseImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			0, 2
		);
		featureData->imageArray->AddImageView(
			"PhillipsSpectrumImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			2, 2
		);
		featureData->imageArray->AddImageView(
			"SpectrumImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			4, 3
		);
		featureData->imageArray->AddImageView(
			"IfftImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			7, 4
		);
		featureData->imageArray->AddImageView(
			"OriginalDisplacementImageGroup",
			VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			8, 3
		);
	}

	{
		featureData->phillipsSpectrumShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_PhillipsSpectrum_Shader.shader");
		featureData->phillipsSpectrumMaterial = new Core::Graphic::Rendering::Material(featureData->phillipsSpectrumShader);
		featureData->phillipsSpectrumMaterial->SetStorageImage2D("gaussianNoiseImageArray", featureData->imageArray, "GaussianNoiseImageGroup");
		featureData->phillipsSpectrumMaterial->SetStorageImage2D("phillipsSpectrumImageArray", featureData->imageArray, "PhillipsSpectrumImageGroup");
	}

	{
		featureData->spectrumShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Spectrum_Shader.shader");
		featureData->spectrumMaterial = new Core::Graphic::Rendering::Material(featureData->spectrumShader);
		featureData->spectrumMaterial->SetStorageImage2D("phillipsSpectrumImageArray", featureData->imageArray, "PhillipsSpectrumImageGroup");
		featureData->spectrumMaterial->SetStorageImage2D("spectrumImageArray", featureData->imageArray, "SpectrumImageGroup");
	}

	{
		featureData->ifftShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Ifft_Shader.shader");
		featureData->ifftMaterial = new Core::Graphic::Rendering::Material(featureData->ifftShader);
		featureData->ifftMaterial->SetStorageImage2D("ifftImageArray", featureData->imageArray, "IfftImageGroup");
	}

	{
		featureData->displacementImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		featureData->normalImage = Core::Graphic::Instance::Image::Create2DImage(
			imageExtent,
			VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		
		featureData->resolveDisplacementShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_ResolveDisplacement_Shader.shader");
		featureData->resolveDisplacementMaterial = new Core::Graphic::Rendering::Material(featureData->resolveDisplacementShader);
		featureData->resolveDisplacementMaterial->SetStorageImage2D("originalDisplacementImageArray", featureData->imageArray, "OriginalDisplacementImageGroup");
		featureData->resolveDisplacementMaterial->SetStorageImage2D("displacementImage", featureData->displacementImage);
		
		featureData->resolveNormalShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_ResolveNormal_Shader.shader");
		featureData->resolveNormalMaterial = new Core::Graphic::Rendering::Material(featureData->resolveNormalShader);
		featureData->resolveNormalMaterial->SetStorageImage2D("displacementImage", featureData->displacementImage);
		featureData->resolveNormalMaterial->SetStorageImage2D("normalImage", featureData->normalImage);

		featureData->surfaceMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Surface.ply");
		featureData->surfaceShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Surface_Shader.shader");
		featureData->surfaceMaterial = new Core::Graphic::Rendering::Material(featureData->surfaceShader);
		featureData->surfaceMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		featureData->surfaceMaterial->SetSampledImage2D("displacementTexture", featureData->displacementImage, _linearSampler);
		featureData->surfaceMaterial->SetSampledImage2D("normalTexture", featureData->normalImage, _linearSampler);
		
		featureData->surfaceWireFrameShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_SurfaceWireFrame_Shader.shader");
		featureData->surfaceWireFrameMaterial = new Core::Graphic::Rendering::Material(featureData->surfaceWireFrameShader);
		featureData->surfaceWireFrameMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
		featureData->surfaceWireFrameMaterial->SetSampledImage2D("displacementTexture", featureData->displacementImage, _linearSampler);
		featureData->surfaceWireFrameMaterial->SetSampledImage2D("normalTexture", featureData->normalImage, _linearSampler);
	}

	return featureData;
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	featureData->launcher->deleteLater();

	delete featureData->frameBuffer;

	delete featureData->gaussianNoiseImageStagingBuffer;

	delete featureData->imageArray;

	delete featureData->phillipsSpectrumMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_PhillipsSpectrum_Shader.shader");

	delete featureData->spectrumMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Spectrum_Shader.shader");

	delete featureData->ifftMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Ifft_Shader.shader");

	delete featureData->displacementImage;
	delete featureData->normalImage;
	delete featureData->resolveDisplacementMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_ResolveDisplacement_Shader.shader");
	delete featureData->resolveNormalMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_ResolveNormal_Shader.shader");

	delete featureData->surfaceMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_Surface_Shader.shader");
	delete featureData->surfaceWireFrameMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\FftOcean_SurfaceWireFrame_Shader.shader");

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
					featureData.imageArray,
					"GaussianNoiseImageGroup",
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

			commandBuffer->CopyBufferToImage(featureData.gaussianNoiseImageStagingBuffer, featureData.imageArray, "GaussianNoiseImageGroup", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			{
				auto gaussianNoiseImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData.imageArray,
					"GaussianNoiseImageGroup",
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
	if(featureData.isDirty)
	{
		featureData.isDirty = false;

		{
			auto phillipsSpectrumImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"PhillipsSpectrumImageGroup",
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
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
			glm::vec2 windDirection;
			float L;
			float windSpeed;
			float a;
			float windDependency;
		};
		PhillipsSpectrumInfo phillipsSpectrumInfo{};
		phillipsSpectrumInfo.imageSize = featureData.imageSize;
		constexpr double PI = 3.141592653589793;
		const auto&& radian = featureData.windRotationAngle / 360 * 2 * PI;
		phillipsSpectrumInfo.windDirection = glm::normalize(glm::vec2(std::cos(radian), std::sin(radian)));
		phillipsSpectrumInfo.L = featureData.L;
		phillipsSpectrumInfo.windSpeed = featureData.windSpeed;
		phillipsSpectrumInfo.a = featureData.a;
		phillipsSpectrumInfo.windDependency = featureData.windDependency;

		commandBuffer->PushConstant(featureData.phillipsSpectrumMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, phillipsSpectrumInfo);
		commandBuffer->Dispatch(featureData.phillipsSpectrumMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);

		{
			auto phillipsSpectrumImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"PhillipsSpectrumImageGroup",
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
	constexpr int IFFT_IMAGE_GROUP_OFFSET = 7;
	int tempImageIndex = 0;
	int heightImageIndex = 1;
	int xImageIndex = 2;
	int yImageIndex = 3;

	// spectrum
	{
		{
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
				{ &tempImageBarrier }
			);
		}

		struct SpectrumInfo
		{
			glm::ivec2 imageSize;
			float L;
			float time;
		};
		SpectrumInfo spectrumInfo{};
		spectrumInfo.imageSize = featureData.imageSize;
		spectrumInfo.L = featureData.L;
		spectrumInfo.time = time;

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
				"OriginalDisplacementImageGroup",
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
			featureData.imageArray, "OriginalDisplacementImageGroup", VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
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
				"ImageView" + std::to_string(tempImageIndex + IFFT_IMAGE_GROUP_OFFSET),
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			auto imageGroupBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.imageArray,
				"OriginalDisplacementImageGroup",
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
			int isLast;
			int isHorizen;
			int blockSize;
			int sourceIndex;
			int targetIndex;
		};
		const int ifftXCount = std::log2(featureData.imageSize.x);
		const int ifftYCount = std::log2(featureData.imageSize.y);

		IfftConstantInfo ifftConstantInfo{};
		ifftConstantInfo.imageSize = featureData.imageSize;

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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
						"ImageView" + std::to_string(ifftConstantInfo.targetIndex + IFFT_IMAGE_GROUP_OFFSET),
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
		{
			auto displacementImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.displacementImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_NONE,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &displacementImageBarrier }
			);
		}

		struct ResolveDisplacementConstantInfo
		{
			glm::vec3 displacementFactor;
			alignas(8) glm::ivec2 imageSize;
		};
		ResolveDisplacementConstantInfo resolveDisplacementConstantInfo{};
		resolveDisplacementConstantInfo.imageSize = featureData.imageSize;
		resolveDisplacementConstantInfo.displacementFactor = featureData.displacementFactor;

		commandBuffer->PushConstant(featureData.resolveDisplacementMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, resolveDisplacementConstantInfo);
		commandBuffer->Dispatch(featureData.resolveDisplacementMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);

		{
			auto displacementImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.displacementImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
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
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				{ &displacementImageBarrier, &normalImageBarrier }
			);
		}

		struct ResolveNormalConstantInfo
		{
			glm::ivec2 imageSize;
			glm::ivec2 meshEdgeVertexCount;
			float normalScale;
			float bubblesLambda;
			float bubblesThreshold;
			float bubblesScale;
		};
		ResolveNormalConstantInfo resolveNormalConstantInfo{};
		resolveNormalConstantInfo.imageSize = featureData.imageSize;
		resolveNormalConstantInfo.meshEdgeVertexCount = { 257, 257 };
		resolveNormalConstantInfo.normalScale = featureData.normalScale;
		resolveNormalConstantInfo.bubblesLambda = featureData.bubblesLambda;
		resolveNormalConstantInfo.bubblesThreshold = featureData.bubblesThreshold;
		resolveNormalConstantInfo.bubblesScale = featureData.bubblesScale;

		commandBuffer->PushConstant(featureData.resolveNormalMaterial, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, resolveNormalConstantInfo);
		commandBuffer->Dispatch(featureData.resolveNormalMaterial, featureData.imageSize.x / LOCAL_GROUP_WIDTH, featureData.imageSize.y / LOCAL_GROUP_WIDTH, 1);
		
		{
			auto displacementImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData.displacementImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT,
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

	// projected grid
	std::array<glm::vec4, 4> uvCorners{};
	if (PopulateUvCornerPositions2(renderFeatureData, camera, uvCorners))
	{
		struct ProjectedGridInfo
		{
			glm::vec4 corner00;
			glm::vec4 corner10;
			glm::vec4 corner01;
			glm::vec4 corner11;
			glm::vec3 scale;
		};
		ProjectedGridInfo projectedGridInfo{};
		projectedGridInfo.corner00 = uvCorners.at(0);
		projectedGridInfo.corner10 = uvCorners.at(1);
		projectedGridInfo.corner01 = uvCorners.at(2);
		projectedGridInfo.corner11 = uvCorners.at(3);
		projectedGridInfo.scale = { featureData.oceanScale, featureData.oceanScale, featureData.oceanScale };

		commandBuffer->BeginRenderPass(_renderPass, featureData.frameBuffer);

		{
			auto material = featureData.showWireFrame ? featureData.surfaceWireFrameMaterial : featureData.surfaceMaterial;
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			commandBuffer->PushConstant(material, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, projectedGridInfo);
			commandBuffer->DrawMesh(featureData.surfaceMesh, material);
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

bool AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::PopulateUvCornerPositions(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera, std::array<glm::vec4, 4>& targets)
{
	auto& featureData = *static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	if (dynamic_cast<Camera::PerspectiveCamera*>(camera) == nullptr)
	{
		Utils::Log::Message("FFT ocean can only use perspective camera.");
	}
	auto& renderCamera = *dynamic_cast<Camera::PerspectiveCamera*>(camera);

	auto& cameraTransform = renderCamera.GameObject()->transform;
	const glm::dmat4&& cameraModelMatrix = cameraTransform.ModelMatrix();
	const glm::dvec3&& cameraPosition = cameraModelMatrix * glm::dvec4(0, 0, 0, 1);
	const glm::dvec3&& cameraInfoForward = glm::normalize(glm::dvec3(cameraModelMatrix * glm::dvec4(0, 0, -1, 0)));

	const double pi = std::acos(-1.0);
	const double halfFov = renderCamera.fovAngle * pi / 360.0;
	double cot = 1.0 / std::tan(halfFov);
	double flatDistence = renderCamera.farFlat - renderCamera.nearFlat;

	const glm::dmat4&& cameraProjectionMatrix = glm::dmat4(
		cot / renderCamera.aspectRatio, 0, 0, 0,
		0, cot, 0, 0,
		0, 0, -renderCamera.farFlat / flatDistence, -1,
		0, 0, -renderCamera.nearFlat * renderCamera.farFlat / flatDistence, 0
	);

	const glm::dvec3&& eye = cameraModelMatrix * glm::dvec4(0, 0, 0, 1);
	const glm::dvec3&& center = cameraModelMatrix * glm::dvec4(0, 0, -1, 1);
	const glm::dvec3&& up = cameraModelMatrix * glm::dvec4(0, 1, 0, 0);

	const glm::dmat4&& cameraViewMatrix = glm::lookAt(eye, center, up);
	const glm::dmat4&& cameraInvViewMatrix = glm::inverse(cameraViewMatrix);

	const glm::dmat4&& cameraViewProjectionMatrix = cameraProjectionMatrix * cameraViewMatrix;
	const glm::dmat4&& cameraInvViewProjectionMatrix = glm::inverse(cameraViewProjectionMatrix);

	std::array<glm::dvec4, 8> worldSpaceCornerPositions{};
	{
		const std::array<glm::dvec4, 8> ndcCornerPositions
		{
			glm::dvec4{-1, -1, 0, 1},
			glm::dvec4{+1, -1, 0, 1},
			glm::dvec4{-1, +1, 0, 1},
			glm::dvec4{+1, +1, 0, 1},
			glm::dvec4{-1, -1, 1, 1},
			glm::dvec4{+1, -1, 1, 1},
			glm::dvec4{-1, +1, 1, 1},
			glm::dvec4{+1, +1, 1, 1},
		};

		for (uint32_t i = 0; i < 8; ++i)
		{
			const auto& ndcCornerPosition = ndcCornerPositions.at(i);
			auto& worldSpaceCornerPosition = worldSpaceCornerPositions.at(i);

			const auto&& Temp = cameraInvViewProjectionMatrix * ndcCornerPosition;
			worldSpaceCornerPosition = Temp / Temp.w;
		}
	}

	const std::array<int, 24> ndcVertexIndexs = {
		0,1,	0,2,	2,3,	1,3,
		0,4,	2,6,	3,7,	1,5,
		4,6,	4,5,	5,7,	6,7
	};

	const glm::dvec3&& planeNormal{ 0, 1, 0 };
	const glm::dvec4&& upperPlane{ 0, 1, 0, -featureData.absDisplacement.y * featureData.oceanScale };
	const glm::dvec4&& lowerPlane{ 0, 1, 0, +featureData.absDisplacement.y * featureData.oceanScale };

	std::vector<glm::dvec3> cameraProjectedPositions{};
	cameraProjectedPositions.reserve(24);
	{
		for (uint32_t i = 0; i < 12; i++)
		{
			const int src = ndcVertexIndexs.at(i * 2), dst = ndcVertexIndexs.at(i * 2 + 1);
			const glm::dvec4& srcCornerPosition = worldSpaceCornerPositions.at(src);
			const glm::dvec4& dstCornerPosition = worldSpaceCornerPositions.at(dst);
			const glm::dvec3&& srcToDstDirection = glm::normalize(glm::dvec3(dstCornerPosition - srcCornerPosition));

			if (glm::dot(upperPlane, srcCornerPosition) * glm::dot(upperPlane, dstCornerPosition) < 0)
			{
				double distance = 0;
				glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -upperPlane.w, 0), planeNormal, distance);
				const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
				cameraProjectedPositions.emplace_back(intersectedPosition);
			}

			if (glm::dot(lowerPlane, srcCornerPosition) * glm::dot(lowerPlane, dstCornerPosition) < 0)
			{
				double distance = 0;
				glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -lowerPlane.w, 0), planeNormal, distance);
				const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
				cameraProjectedPositions.emplace_back(intersectedPosition);
			}
		}

		for (int i = 0; i < 8; i++)
		{
			const glm::dvec4& cornerPosition = worldSpaceCornerPositions.at(i);
			if (glm::dot(upperPlane, cornerPosition) * glm::dot(lowerPlane, cornerPosition) < 0)
			{
				cameraProjectedPositions.emplace_back(glm::dvec3(cornerPosition));
			}
		}

		for (auto& cameraProjectedPosition : cameraProjectedPositions)
		{
			cameraProjectedPosition = cameraProjectedPosition - planeNormal * glm::dot(planeNormal, cameraProjectedPosition);
		}

		cameraProjectedPositions.reserve(cameraProjectedPositions.size() * 5);
		const glm::dvec3 xDisplacement(featureData.oceanScale * featureData.absDisplacement.x, 0, 0);
		const glm::dvec3 zDisplacement(0, 0, featureData.oceanScale * featureData.absDisplacement.z);
		for (int i = 0, len = cameraProjectedPositions.size(); i < len; ++i)
		{
			const auto& cameraProjectedPosition = cameraProjectedPositions.at(i);
			cameraProjectedPositions.emplace_back(cameraProjectedPosition + xDisplacement);
			cameraProjectedPositions.emplace_back(cameraProjectedPosition - xDisplacement);
			cameraProjectedPositions.emplace_back(cameraProjectedPosition + zDisplacement);
			cameraProjectedPositions.emplace_back(cameraProjectedPosition - zDisplacement);
		}

		for (auto& cameraProjectedPosition : cameraProjectedPositions)
		{
			glm::dvec4 temp = cameraViewProjectionMatrix * glm::dvec4(cameraProjectedPosition, 1);
			cameraProjectedPosition = temp / temp.w;
		}
	}

	glm::dmat4 rangeMatrix{};
	bool needRenderWater;
	if (cameraProjectedPositions.size() == 0)
	{
		return false;
	}
	else
	{
		double x_min = cameraProjectedPositions.at(0).x;
		double x_max = cameraProjectedPositions.at(0).x;
		double y_min = cameraProjectedPositions.at(0).y;
		double y_max = cameraProjectedPositions.at(0).y;
		for (int i = 1; i < cameraProjectedPositions.size(); i++)
		{
			const auto& cameraProjectedPosition = cameraProjectedPositions.at(i);
			x_min = std::min(x_min, cameraProjectedPosition.x);
			x_max = std::max(x_max, cameraProjectedPosition.x);
			y_min = std::min(y_min, cameraProjectedPosition.y);
			y_max = std::max(y_max, cameraProjectedPosition.y);
		}

		rangeMatrix = {
			x_max - x_min, 0, 0, 0,
			0, y_max - y_min, 0, 0,
			0, 0, 1, 0,
			x_min, y_min, 0, 1
		};
	}

	const glm::dmat4&& rangeInvViewProjectionMatrix = cameraInvViewProjectionMatrix * rangeMatrix;

	{
		{
			double u = 0;
			double v = 0;
			auto& uvCorner = targets.at(0);

			glm::dvec4 origin(u, v, 0, 1);
			glm::dvec4 direction(u, v, 1, 1);

			origin = rangeInvViewProjectionMatrix * origin;
			direction = rangeInvViewProjectionMatrix * direction;
			direction = direction - origin;

			double l = -origin.y / direction.y;

			uvCorner = origin + direction * l;
		}
		{
			double u = 1;
			double v = 0;
			auto& uvCorner = targets.at(1);

			glm::dvec4 origin(u, v, 0, 1);
			glm::dvec4 direction(u, v, 1, 1);

			origin = rangeInvViewProjectionMatrix * origin;
			direction = rangeInvViewProjectionMatrix * direction;
			direction = direction - origin;

			double l = -origin.y / direction.y;

			uvCorner = origin + direction * l;
		}
		{
			double u = 0;
			double v = 1;
			auto& uvCorner = targets.at(2);

			glm::dvec4 origin(u, v, 0, 1);
			glm::dvec4 direction(u, v, 1, 1);

			origin = rangeInvViewProjectionMatrix * origin;
			direction = rangeInvViewProjectionMatrix * direction;
			direction = direction - origin;

			double l = -origin.y / direction.y;

			uvCorner = origin + direction * l;
		}
		{
			double u = 1;
			double v = 1;
			auto& uvCorner = targets.at(3);

			glm::dvec4 origin(u, v, 0, 1);
			glm::dvec4 direction(u, v, 1, 1);

			origin = rangeInvViewProjectionMatrix * origin;
			direction = rangeInvViewProjectionMatrix * direction;
			direction = direction - origin;

			double l = -origin.y / direction.y;

			uvCorner = origin + direction * l;
		}
	}

	return true;
}

bool AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::PopulateUvCornerPositions2(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera, std::array<glm::vec4, 4>& targets)
{
	auto& featureData = *static_cast<FftOcean_RenderFeatureData*>(renderFeatureData);

	if (dynamic_cast<Camera::PerspectiveCamera*>(camera) == nullptr)
	{
		Utils::Log::Message("FFT ocean can only use perspective camera.");
	}
	auto& renderCamera = *dynamic_cast<Camera::PerspectiveCamera*>(camera);

	auto& cameraTransform = renderCamera.GameObject()->transform;
	const glm::dmat4&& cameraModelMatrix = cameraTransform.ModelMatrix();
	const glm::dvec3&& cameraPosition = cameraModelMatrix * glm::dvec4(0, 0, 0, 1);
	const glm::dvec3&& cameraLookAtPosition = cameraModelMatrix * glm::dvec4(0, 0, -1, 1);
	const glm::dvec3&& cameraUp = cameraModelMatrix * glm::dvec4(0, 1, 0, 0);

	const double PI = std::acos(-1.0);
	const double&& halfFov = renderCamera.fovAngle * PI / 360.0;
	const double&& cot = 1.0 / std::tan(halfFov);
	const double&& flatDistence = renderCamera.farFlat - renderCamera.nearFlat;

	const glm::dmat4&& cameraProjectionMatrix = glm::dmat4(
		cot / renderCamera.aspectRatio	, 0		, 0																, 0,
		0								, cot	, 0																, 0,
		0								, 0		, -renderCamera.farFlat / flatDistence							, -1,
		0								, 0		, -renderCamera.nearFlat * renderCamera.farFlat / flatDistence	, 0
	);

	const glm::dmat4&& cameraViewMatrix = glm::lookAt(cameraPosition, cameraLookAtPosition, cameraUp);
	const glm::dmat4&& cameraInvViewMatrix = glm::inverse(cameraViewMatrix);

	const glm::dmat4&& cameraViewProjectionMatrix = cameraProjectionMatrix * cameraViewMatrix;
	const glm::dmat4&& cameraInvViewProjectionMatrix = glm::inverse(cameraViewProjectionMatrix);

	std::array<glm::dvec4, 8> worldSpaceCornerPositions{};
	{
		const std::array<glm::dvec4, 8> ndcCornerPositions
		{
			glm::dvec4{-1, -1, 0, 1},
			glm::dvec4{+1, -1, 0, 1},
			glm::dvec4{-1, +1, 0, 1},
			glm::dvec4{+1, +1, 0, 1},
			glm::dvec4{-1, -1, 1, 1},
			glm::dvec4{+1, -1, 1, 1},
			glm::dvec4{-1, +1, 1, 1},
			glm::dvec4{+1, +1, 1, 1},
		};

		for (uint32_t i = 0; i < 8; ++i)
		{
			const auto& ndcCornerPosition = ndcCornerPositions.at(i);
			auto& worldSpaceCornerPosition = worldSpaceCornerPositions.at(i);

			worldSpaceCornerPosition = cameraInvViewProjectionMatrix * ndcCornerPosition;
			worldSpaceCornerPosition = worldSpaceCornerPosition / worldSpaceCornerPosition.w;
		}
	}

	const std::array<int, 24> ndcVertexIndexs = {
		0,1,	0,2,	2,3,	1,3,
		0,4,	2,6,	3,7,	1,5,
		4,6,	4,5,	5,7,	6,7
	};

	const glm::dvec3&& planeNormal{ 0, 1, 0 };
	const glm::dvec4&& upperPlane{ 0, 1, 0, -featureData.absDisplacement.y * featureData.oceanScale };
	const glm::dvec4&& lowerPlane{ 0, 1, 0, +featureData.absDisplacement.y * featureData.oceanScale };

	std::vector<glm::dvec3> cameraProjectedPositions{};
	cameraProjectedPositions.reserve(32);
	{
		for (uint32_t i = 0; i < 12; i++)
		{
			const int src = ndcVertexIndexs.at(i * 2), dst = ndcVertexIndexs.at(i * 2 + 1);
			const glm::dvec4& srcCornerPosition = worldSpaceCornerPositions.at(src);
			const glm::dvec4& dstCornerPosition = worldSpaceCornerPositions.at(dst);
			const glm::dvec3&& srcToDstDirection = glm::normalize(glm::dvec3(dstCornerPosition - srcCornerPosition));

			double distance = 0;
			if (glm::dot(upperPlane, srcCornerPosition) * glm::dot(upperPlane, dstCornerPosition) < 0)
			{
				if (glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -upperPlane.w, 0), planeNormal, distance))
				{
					const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
					cameraProjectedPositions.emplace_back(intersectedPosition);
				}
			}

			if (glm::dot(lowerPlane, srcCornerPosition) * glm::dot(lowerPlane, dstCornerPosition) < 0)
			{
				if (glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -lowerPlane.w, 0), planeNormal, distance))
				{
					const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
					cameraProjectedPositions.emplace_back(intersectedPosition);
				}
			}
		}

		for (int i = 0; i < 8; i++)
		{
			const glm::dvec4& cornerPosition = worldSpaceCornerPositions.at(i);
			if (glm::dot(upperPlane, cornerPosition) * glm::dot(lowerPlane, cornerPosition) < 0)
			{
				cameraProjectedPositions.emplace_back(glm::dvec3(cornerPosition));
			}
		}

		for (auto& cameraProjectedPosition : cameraProjectedPositions)
		{
			cameraProjectedPosition = cameraProjectedPosition - planeNormal * glm::dot(planeNormal, cameraProjectedPosition);
			glm::dvec4 temp = cameraViewProjectionMatrix * glm::dvec4(cameraProjectedPosition, 1);
			cameraProjectedPosition = temp / temp.w;
		}
	}

	glm::dmat4 rangeMatrix{};
	if (cameraProjectedPositions.size() == 0)
	{
		return false;
	}
	else
	{
		double x_min = cameraProjectedPositions.at(0).x;
		double x_max = cameraProjectedPositions.at(0).x;
		double y_min = cameraProjectedPositions.at(0).y;
		double y_max = cameraProjectedPositions.at(0).y;
		for (int i = 1; i < cameraProjectedPositions.size(); i++)
		{
			const auto& cameraProjectedPosition = cameraProjectedPositions.at(i);
			x_min = std::min(x_min, cameraProjectedPosition.x);
			x_max = std::max(x_max, cameraProjectedPosition.x);
			y_min = std::min(y_min, cameraProjectedPosition.y);
			y_max = std::max(y_max, cameraProjectedPosition.y);
		}

		rangeMatrix = {
			x_max - x_min	, 0				, 0, 0,
			0				, y_max - y_min	, 0, 0,
			0				, 0				, 1, 0,
			x_min			, y_min			, 0, 1
		};
	}

	const glm::dmat4&& rangeInvViewProjectionMatrix = cameraInvViewProjectionMatrix * rangeMatrix;

	{
		const std::array<glm::dvec2, 4> uvCorners
		{
			glm::dvec2{0, 0},
			glm::dvec2{1, 0},
			glm::dvec2{0, 1},
			glm::dvec2{1, 1}
		};

		std::array<glm::dvec4, 4> worldSpaceOriginalRangeHomoPositions{};
		std::array<glm::dvec3, 4> worldSpaceOriginalRangePositions{};
		for (uint32_t i = 0; i < 4; ++i)
		{
			const auto& uvCorner = uvCorners.at(i);
			auto& worldSpaceOriginalRangeHomoPosition = worldSpaceOriginalRangeHomoPositions.at(i);
			auto& worldSpaceOriginalRangePosition = worldSpaceOriginalRangePositions.at(i);

			auto&& srcPosition = rangeInvViewProjectionMatrix * glm::dvec4{ uvCorner, 0, 1 };
			auto&& dstPosition = rangeInvViewProjectionMatrix * glm::dvec4{ uvCorner, 1, 1 };
			auto&& direction = dstPosition - srcPosition;

			auto&& ratio = -srcPosition.y / direction.y;   

			worldSpaceOriginalRangeHomoPosition = srcPosition + direction * ratio;

			worldSpaceOriginalRangePosition = worldSpaceOriginalRangeHomoPosition / worldSpaceOriginalRangeHomoPosition.w;
		}

		const glm::dvec3&& RightDirection = glm::normalize(worldSpaceOriginalRangePositions.at(1) - worldSpaceOriginalRangePositions.at(0));
		glm::dvec3 ForwardDirection = glm::normalize(glm::cross({ 0, 1, 0 }, RightDirection));

		const double DisplacementX = featureData.oceanScale * featureData.absDisplacement.x * featureData.displacementFactor.x;
		const double DisplacementZ = featureData.oceanScale * featureData.absDisplacement.z * featureData.displacementFactor.z;
		const double Displacement = std::max(DisplacementX, DisplacementZ);

		std::array<glm::dvec3, 4> worldSpaceDisplacedRangePositions{};
		worldSpaceDisplacedRangePositions.at(0) = worldSpaceOriginalRangePositions.at(0) - Displacement * RightDirection - Displacement * ForwardDirection;
		worldSpaceDisplacedRangePositions.at(1) = worldSpaceOriginalRangePositions.at(1) + Displacement * RightDirection - Displacement * ForwardDirection;
		worldSpaceDisplacedRangePositions.at(2) = worldSpaceOriginalRangePositions.at(2) - Displacement * RightDirection + Displacement * ForwardDirection;
		worldSpaceDisplacedRangePositions.at(3) = worldSpaceOriginalRangePositions.at(3) + Displacement * RightDirection + Displacement * ForwardDirection;
		//worldSpaceDisplacedRangePositions.at(0) = worldSpaceOriginalRangePositions.at(0);
		//worldSpaceDisplacedRangePositions.at(1) = worldSpaceOriginalRangePositions.at(1);
		//worldSpaceDisplacedRangePositions.at(2) = worldSpaceOriginalRangePositions.at(2);
		//worldSpaceDisplacedRangePositions.at(3) = worldSpaceOriginalRangePositions.at(3);

		std::array<glm::dvec4, 4> worldSpaceDisplacedRangeHomoPositions{};
		for (uint32_t i = 0; i < 4; ++i)
		{
			auto& targetRangePosition = worldSpaceDisplacedRangeHomoPositions.at(i);
			const auto& worldSpaceDisplacedRangePosition = worldSpaceDisplacedRangePositions.at(i);

			targetRangePosition = cameraViewProjectionMatrix * glm::dvec4(worldSpaceDisplacedRangePosition, 1);
			targetRangePosition = targetRangePosition / targetRangePosition.w;
			targetRangePosition = cameraInvViewProjectionMatrix * targetRangePosition;
		}

		for (uint32_t i = 0; i < 4; ++i)
		{
			targets.at(i) = worldSpaceDisplacedRangeHomoPositions.at(i);
		}
	}

	return true;
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
			fftOceanDataPtr->isDirty = true;
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
			fftOceanDataPtr->isDirty = true;
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
			fftOceanDataPtr->isDirty = true;
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
			fftOceanDataPtr->isDirty = true;
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
			fftOceanDataPtr->isDirty = true;
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

	// absDisplacement.height
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->absDisplacement.y)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->absDisplacement.y = string.toFloat();
			Utils::Log::Message("absDisplacement.height: " + std::to_string(fftOceanDataPtr->absDisplacement.y));
		});
		pLayout->addRow(QStringLiteral("absDisplacement.height: "), lineEdit);
	}

	// absDisplacement.x
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->absDisplacement.x)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->absDisplacement.x = string.toFloat();
			Utils::Log::Message("absDisplacement.x: " + std::to_string(fftOceanDataPtr->absDisplacement.x));
		});
		pLayout->addRow(QStringLiteral("absDisplacement.x: "), lineEdit);
	}

	// absDisplacement.z
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(fftOceanDataPtr->absDisplacement.z)), this);
		lineEdit->setValidator(doubleValidator);
		lineEdit->connect(lineEdit, &QLineEdit::textChanged, this, [fftOceanDataPtr](const QString& string)->void {
			fftOceanDataPtr->absDisplacement.z = string.toFloat();
			Utils::Log::Message("absDisplacement.z: " + std::to_string(fftOceanDataPtr->absDisplacement.z));
		});
		pLayout->addRow(QStringLiteral("absDisplacement.z: "), lineEdit);
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

	// showWireFrame
	{
		QCheckBox* checkBox = new QCheckBox(this);
		checkBox->setCheckState(fftOceanDataPtr->showWireFrame ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		checkBox->connect(checkBox, &QCheckBox::stateChanged, this, [fftOceanDataPtr](int state)->void {
			fftOceanDataPtr->showWireFrame = (state != Qt::CheckState::Unchecked);
			Utils::Log::Message(fftOceanDataPtr->showWireFrame ? "showWireFrame: true" : "showWireFrame: false");
		});
		pLayout->addRow(QStringLiteral("showWireFrame: "), checkBox);
	}

	setLayout(pLayout);
}
