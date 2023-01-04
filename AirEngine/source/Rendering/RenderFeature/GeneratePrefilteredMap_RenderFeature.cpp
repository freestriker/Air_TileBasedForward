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

constexpr uint32_t X_POSITIVE_INDEX = 0;
constexpr uint32_t X_NEGATIVE_INDEX = 1;
constexpr uint32_t Y_POSITIVE_INDEX = 2;
constexpr uint32_t Y_NEGATIVE_INDEX = 3;
constexpr uint32_t Z_POSITIVE_INDEX = 4;
constexpr uint32_t Z_NEGATIVE_INDEX = 5;

constexpr std::array<glm::vec3, 6> FACE_ROTATIONS
{
	glm::vec3{ 0, -90, 0 },
	glm::vec3{ 0, 90, 0 },
	glm::vec3{ 90, 0, 0 },
	glm::vec3{ -90, 0, 0 },
	glm::vec3{ 0, 0, 0 },
	glm::vec3{ 0, 180, 0 }
};

const static glm::mat4 GET_FACE_VIEW_MATRIX(const uint32_t faceIndex)
{
	const glm::vec3 rotation = FACE_ROTATIONS[faceIndex];
	const glm::mat4 model = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), glm::radians(rotation.x), { 1, 0, 0 }), glm::radians(rotation.y), { 0, 1, 0 }), glm::radians(rotation.z), { 0, 0, 1 });
	const glm::vec3 eye = model * glm::vec4(0, 0, 0, 1);
	const glm::vec3 center = glm::normalize(model * glm::vec4(0, 0, -1, 1));
	const glm::vec3 up = glm::normalize(glm::vec3(model * glm::vec4(0, 1, 0, 0)));
	const glm::mat4 view = glm::lookAt(eye, center, up);
	return view;
}
const static glm::mat4 GET_PROJECTION_MATRIX()
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
	return projection;
}

const static std::array<glm::mat4, 6> FACE_VIEW_PROJECTION_MATRIXES
{
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(0),
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(1),
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(2),
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(3),
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(4),
	GET_PROJECTION_MATRIX() * GET_FACE_VIEW_MATRIX(5)
};

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderPass>("AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderPass")
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

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderPass::GeneratePrefilteredMap_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderPass::~GeneratePrefilteredMap_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	, _generateMaterial(nullptr)
	, _targetCubeImage(nullptr)
	, _environmentImage(nullptr)
	, _weightInfoBuffer(nullptr)
	, _boxMesh(nullptr)
	, _generateShader(nullptr)
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
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GeneratePrefilteredMap_RenderPass>())
{

}

AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::~GeneratePrefilteredMap_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GeneratePrefilteredMap_RenderPass>();
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

	featureData->_targetCubeImage = Core::Graphic::Instance::Image::CreateCubeImage(
		featureData->resolution,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
		featureData->roughnessLevelCount
	);

	featureData->_weightInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(WeightInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	featureData->_frameBuffers.resize(featureData->roughnessLevelCount * 6);
	for (uint32_t faceIndex = X_POSITIVE_INDEX; faceIndex <= Z_NEGATIVE_INDEX; faceIndex++)
	{
		for (uint32_t roughnessIndex = 0; roughnessIndex < featureData->roughnessLevelCount; roughnessIndex++)
		{
			featureData->_targetCubeImage->AddImageView(
				"ColorAttachmentView_Face" + std::to_string(faceIndex) + "_Roughness" + std::to_string(roughnessIndex),
				VK_IMAGE_VIEW_TYPE_2D,
				VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
				faceIndex,
				1,
				roughnessIndex,
				1
			);
			featureData->_frameBuffers[faceIndex * featureData->roughnessLevelCount + roughnessIndex] = new Core::Graphic::Rendering::FrameBuffer(
				_renderPass,
				{
					{"ColorAttachment", featureData->_targetCubeImage}
				},
				{
					{"ColorAttachment", "ColorAttachmentView_Face" + std::to_string(faceIndex) + "_Roughness" + std::to_string(roughnessIndex)}
				}
			);
		}
	}

	featureData->_environmentImageSampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_LINEAR,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	featureData->_generateShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GeneratePrefilteredMap_Shader.shader");
	featureData->_generateMaterial = new Core::Graphic::Rendering::Material(featureData->_generateShader);
	featureData->_generateMaterial->SetSampledImageCube("environmentImage", featureData->_environmentImage, featureData->_environmentImageSampler);
	featureData->_generateMaterial->SetStorageBuffer("weightInfo", featureData->_weightInfoBuffer);

	featureData->_sliceIndex = 0;
}

void AirEngine::Rendering::RenderFeature::GeneratePrefilteredMap_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GeneratePrefilteredMap_RenderFeatureData*>(renderFeatureData);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_generateShader);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_boxMesh);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_environmentImage);
	for (uint32_t faceIndex = X_POSITIVE_INDEX; faceIndex <= Z_NEGATIVE_INDEX; faceIndex++)
	{
		for (uint32_t roughnessIndex = 0; roughnessIndex < featureData->roughnessLevelCount; roughnessIndex++)
		{
			delete featureData->_frameBuffers[faceIndex * featureData->roughnessLevelCount + roughnessIndex];
		}
	}
	delete featureData->_targetCubeImage;
	delete featureData->_weightInfoBuffer;
	delete featureData->_environmentImageSampler;
	delete featureData->_generateMaterial;
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
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &colorAttachmentBarrier }
				);
			}

			commandBuffer->ClearColorImage(featureData->_targetCubeImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { 0.0f, 0.0f, 0.0f, 0.0f });

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
					featureData->_weightInfoBuffer,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineBufferBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &bufferBarrier }
				);
			}

			float clear = 0.0f;
			commandBuffer->FillBuffer(featureData->_weightInfoBuffer, reinterpret_cast<uint32_t&>(clear));

			{
				auto bufferBarrier = Core::Graphic::Command::BufferMemoryBarrier
				(
					featureData->_weightInfoBuffer,
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

	//if (featureData->_sliceIndex < featureData->sliceCount)
	{
		AddPushConstantInfo accumulateInfo{};
		accumulateInfo.stepCount = featureData->stepCount;
		accumulateInfo.sliceCount = featureData->sliceCount;
		accumulateInfo.sliceIndex = featureData->_sliceIndex;
		accumulateInfo.resolution = featureData->resolution.width;

		for (uint32_t faceIndex = X_POSITIVE_INDEX; faceIndex <= Z_NEGATIVE_INDEX; faceIndex++)
		{
			accumulateInfo.faceIndex = faceIndex;
			accumulateInfo.viewProjection = FACE_VIEW_PROJECTION_MATRIXES[faceIndex];

			for (uint32_t roughnessIndex = 0; roughnessIndex < featureData->roughnessLevelCount; roughnessIndex++)
			{
				accumulateInfo.roughness = static_cast<float>(roughnessIndex) / (featureData->roughnessLevelCount - 1);

				commandBuffer->BeginRenderPass(_renderPass, featureData->_frameBuffers[faceIndex * featureData->roughnessLevelCount + roughnessIndex]);
				commandBuffer->PushConstant(featureData->_generateMaterial, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, accumulateInfo);
				commandBuffer->DrawMesh(featureData->_boxMesh, featureData->_generateMaterial);
				commandBuffer->EndRenderPass();
			}
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
