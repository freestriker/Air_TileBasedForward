#include "Rendering/RenderFeature/GeneratePrefilteredMap_RenderFeature.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
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
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>

#define X_POSITIVE_INDEX 0
#define X_NEGATIVE_INDEX 1
#define Y_POSITIVE_INDEX 2
#define Y_NEGATIVE_INDEX 3
#define Z_POSITIVE_INDEX 4
#define Z_NEGATIVE_INDEX 5

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GenerateIrradianceMap_RenderPass>("AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GenerateIrradianceMap_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeatureData>("AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature>("AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GenerateIrradianceMap_RenderPass::GenerateIrradianceMap_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GenerateIrradianceMap_RenderPass::~GenerateIrradianceMap_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GenerateIrradianceMap_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_SAMPLE_COUNT_1_BIT,
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
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0
	);
}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeatureData::GeneratePrefilteredMap_RenderFeatureData()
	: RenderFeatureDataBase()
	, _frameBuffers()
	, _generateMaterial0(nullptr)
	, _generateMaterial1(nullptr)
	, _targetCubeImage(nullptr)
	, _environmentImage(nullptr)
	, _infoBuffer(nullptr)
	, _boxMesh(nullptr)
	, _planeMesh(nullptr)
	, _generateShader0(nullptr)
	, _generateShader1(nullptr)
	, _environmentImageSampler()
	, resolution({ 256, 256 })
	, stepCount(256 * 256)
	, roughnessLevelCount(5)
	, sliceCount(256)
{

}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeatureData::~GeneratePrefilteredMap_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GenerateIrradianceMap_RenderPass>())
{

}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::~GeneratePrefilteredMap_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GenerateIrradianceMap_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	return new GeneratePrefilteredMap_RenderFeatureData();
}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<GeneratePrefilteredMap_RenderFeatureData*>(renderFeatureData);

	featureData->_environmentImage = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(featureData->environmentImagePath);
	featureData->_boxMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Box.ply");
	featureData->_planeMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Plane.ply");
	featureData->_targetCubeImage = Core::Graphic::Instance::Image::CreateCubeImage(
		featureData->resolution,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL
	);
	featureData->_infoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(Info),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	for (int i = X_POSITIVE_INDEX; i <= Z_NEGATIVE_INDEX; i++)
	{
		featureData->_targetCubeImage->AddImageView(
			"ColorAttachmentView_" + std::to_string(i),
			VK_IMAGE_VIEW_TYPE_2D,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			i,
			1
		);
		featureData->_frameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			_renderPass,
			{
				{"ColorAttachment", featureData->_targetCubeImage}
			},
			{
				{"ColorAttachment", "ColorAttachmentView_" + std::to_string(i)}
			}
			);
	}
	featureData->_environmentImageSampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_LINEAR,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	featureData->_generateShader0 = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GeneratePrefilteredMap_Add_Shader.shader");
	featureData->_generateMaterial0 = new Core::Graphic::Rendering::Material(featureData->_generateShader0);
	featureData->_generateMaterial0->SetSampledImageCube("environmentImage", featureData->_environmentImage, featureData->_environmentImageSampler);
	featureData->_generateMaterial0->SetStorageBuffer("info", featureData->_infoBuffer);
	
	featureData->_generateShader1 = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GeneratePrefilteredMap_Division_Shader.shader");
	featureData->_generateMaterial1 = new Core::Graphic::Rendering::Material(featureData->_generateShader1);
	featureData->_generateMaterial1->SetStorageBuffer("info", featureData->_infoBuffer);

	featureData->_sliceIndex = 0;
}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GeneratePrefilteredMap_RenderFeatureData*>(renderFeatureData);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_generateShader0);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_generateShader1);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_boxMesh);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_planeMesh);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_environmentImage);
	for (int i = X_POSITIVE_INDEX; i <= Z_NEGATIVE_INDEX; i++)
	{
		delete featureData->_frameBuffers[i];
	}
	delete featureData->_targetCubeImage;
	delete featureData->_environmentImageSampler;
	delete featureData->_generateMaterial0;
	delete featureData->_generateMaterial1;
}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<GeneratePrefilteredMap_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Clear at first time
	if (featureData->_sliceIndex == 0)
	{
		///Clear image
		{
			{
				auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_targetCubeImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &colorAttachmentBarrier }
				);
			}

			VkClearColorValue clearValue = { 0.0, 0.0, 0.0, 1.0 };
			commandBuffer->ClearColorImage(featureData->_targetCubeImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearValue);

			{
				auto colorAttachmentBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_targetCubeImage,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					{ &colorAttachmentBarrier }
				);
			}
		}

		///Clear info buffer
		{
			{
				auto bufferBarrier = Core::Graphic::Command::BufferMemoryBarrier
				(
					featureData->_infoBuffer,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineBufferBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &bufferBarrier }
				);
			}

			float clear = 0.0f;
			commandBuffer->FillBuffer(featureData->_infoBuffer, reinterpret_cast<uint32_t&>(clear));

			{
				auto bufferBarrier = Core::Graphic::Command::BufferMemoryBarrier
				(
					featureData->_infoBuffer,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
				);
				commandBuffer->AddPipelineBufferBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					{ &bufferBarrier }
				);
			}
		}
	}

	if (featureData->_sliceIndex < featureData->sliceCount)
	{
		for (int faceIndex = X_POSITIVE_INDEX; faceIndex <= Z_NEGATIVE_INDEX; faceIndex++)
		{
			PushConstantInfo generateInfo{};
			///Calculate matrix
			{
				const float aspectRatio = 1;
				const float farFlat = 10;
				const float nearFlat = 0.5;
				const float flatDistence = farFlat - nearFlat;
				const double halfFov = 90 * std::acos(-1.0) / 360.0;
				const double cot = 1.0 / std::tan(halfFov);
				const auto projection = glm::mat4(
					cot / aspectRatio, 0, 0, 0,
					0, cot, 0, 0,
					0, 0, -farFlat / flatDistence, -1,
					0, 0, -nearFlat * farFlat / flatDistence, 0
				);

				glm::vec3 rotation{};
				switch (faceIndex)
				{
					case X_POSITIVE_INDEX:
					{
						rotation = { 0, -90, 0 };
						break;
					}
					case X_NEGATIVE_INDEX:
					{
						rotation = { 0, 90, 0 };
						break;
					}
					case Y_POSITIVE_INDEX:
					{
						rotation = { 90, 0, 0 };
						break;
					}
					case Y_NEGATIVE_INDEX:
					{
						rotation = { -90, 0, 0 };
						break;
					}
					case Z_POSITIVE_INDEX:
					{
						rotation = { 0, 0, 0 };
						break;
					}
					case Z_NEGATIVE_INDEX:
					{
						rotation = { 0, 180, 0 };
						break;
					}
				}
				auto model = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), glm::radians(rotation.x), { 1, 0, 0 }), glm::radians(rotation.y), { 0, 1, 0 }), glm::radians(rotation.z), { 0, 0, 1 });
				glm::vec3 eye = model * glm::vec4(0, 0, 0, 1);
				glm::vec3 center = glm::normalize(model * glm::vec4(0, 0, -1, 1));
				glm::vec3 up = glm::normalize(glm::vec3(model * glm::vec4(0, 1, 0, 0)));
				auto view = glm::lookAt(eye, center, up);

				generateInfo.stepCount = featureData->stepCount;
				generateInfo.sliceCount = featureData->sliceCount;
				generateInfo.sliceIndex = featureData->_sliceIndex;
				generateInfo.resolution = featureData->resolution.width;
				generateInfo.faceIndex = faceIndex;
				generateInfo.viewProjection = projection * view;
			}

			commandBuffer->BeginRenderPass(_renderPass, featureData->_frameBuffers[faceIndex]);
			commandBuffer->PushConstant(featureData->_generateMaterial0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, generateInfo);
			commandBuffer->DrawMesh(featureData->_boxMesh, featureData->_generateMaterial0);
			commandBuffer->EndRenderPass();
		}
	}

	if (featureData->_sliceIndex >= featureData->sliceCount)
	{
		PushConstantInfo generateInfo{};
		generateInfo.stepCount = featureData->stepCount;
		generateInfo.sliceCount = featureData->sliceCount;
		generateInfo.sliceIndex = featureData->_sliceIndex;
		generateInfo.resolution = featureData->resolution.width;
		generateInfo.viewProjection = {};

		///Wait info buffer finished
		{
			auto bufferBarrier = Core::Graphic::Command::BufferMemoryBarrier
			(
				featureData->_infoBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			commandBuffer->AddPipelineBufferBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &bufferBarrier }
			);
		}

		for (int faceIndex = X_POSITIVE_INDEX; faceIndex <= Z_NEGATIVE_INDEX; faceIndex++)
		{
			generateInfo.faceIndex = faceIndex;

			commandBuffer->BeginRenderPass(_renderPass, featureData->_frameBuffers[faceIndex]);
			commandBuffer->PushConstant(featureData->_generateMaterial0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, generateInfo);
			commandBuffer->DrawMesh(featureData->_planeMesh, featureData->_generateMaterial1);
			commandBuffer->EndRenderPass();
		}
	}

	commandBuffer->EndRecord();

	if (featureData->_sliceIndex > featureData->sliceCount)
	{
		Utils::Log::Message("Generating prefiltered environment map is completed.");
	}
	else
	{
		featureData->_sliceIndex++;
	}

}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
