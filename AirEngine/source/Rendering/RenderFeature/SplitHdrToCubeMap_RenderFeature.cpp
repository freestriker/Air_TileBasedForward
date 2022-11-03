#include "Rendering/RenderFeature/SplitHdrToCubeMap_RenderFeature.h"
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
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>

#define RIGHT_INDEX 0
#define LEFT_INDEX 1
#define TOP_INDEX 2
#define BOTTOM_INDEX 3
#define FRONT_INDEX 4
#define BACK_INDEX 5

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderPass>("AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData>("AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature>("AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderPass::SplitHdrToCubeMap_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderPass::~SplitHdrToCubeMap_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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

AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData::SplitHdrToCubeMap_RenderFeatureData()
	: RenderFeatureDataBase()
	, _frameBuffers()
	, _splitMaterial(nullptr)
	, _targetCubeImage(nullptr)
	, _sourceImage(nullptr)
	, _cubeMesh(nullptr)
	, resolution()
	, hdrTexturePath()
{

}

AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData::~SplitHdrToCubeMap_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<SplitHdrToCubeMap_RenderPass>())
{

}

AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::~SplitHdrToCubeMap_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<SplitHdrToCubeMap_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	return new SplitHdrToCubeMap_RenderFeatureData();
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<SplitHdrToCubeMap_RenderFeatureData*>(renderFeatureData);
	featureData->_sourceImage = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(featureData->hdrTexturePath);
	featureData->_splitShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\SplitHdrToCubeMap_Shader.shader");
	featureData->_cubeMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Quad.ply");
	featureData->_targetCubeImage = Core::Graphic::Instance::Image::CreateCubeImage(
		featureData->resolution,
		featureData->format,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL
	);
	for (int i = RIGHT_INDEX; i <= BACK_INDEX; i++)
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
	featureData->_sourceImageSampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_LINEAR,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);
	featureData->_splitMaterial = new Core::Graphic::Rendering::Material(featureData->_splitShader);
	featureData->_splitMaterial->SetSampledImage2D("sourceImage", featureData->_sourceImage, featureData->_sourceImageSampler);
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<SplitHdrToCubeMap_RenderFeatureData*>(renderFeatureData);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_sourceImage);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_splitShader);
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_cubeMesh);
	for (int i = RIGHT_INDEX; i <= BACK_INDEX; i++)
	{
		delete featureData->_frameBuffers[i];
	}
	delete featureData->_targetCubeImage;
	delete featureData->_sourceImageSampler;
	delete featureData->_splitMaterial;
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<SplitHdrToCubeMap_RenderFeatureData*>(renderFeatureData);

	struct SplitInfo
	{
		glm::mat4 viewProjection;
	};
	{
		const float aspectRatio = 1;
		const float farFlat = 10;
		const float nearFlat = 1;
		double halfFov = 90 * std::acos(-1.0) / 360.0;
		double cot = 1.0 / std::tan(halfFov);
		float flatDistence = 10.0 - 0.1;

		auto projection = glm::mat4(
			cot / aspectRatio, 0, 0, 0,
			0, cot, 0, 0,
			0, 0, -farFlat / flatDistence, -1,
			0, 0, -nearFlat * farFlat / flatDistence, 0
		);

		glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), 0.0f, { 1, 0, 0 }), 90.0f, { 0, 1, 0 }), 0.0f, { 0, 0, 1 });
	}

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	for (int i = RIGHT_INDEX; i <= BACK_INDEX; i++)
	{
		SplitInfo splitInfo{};

		///Calculate matrix
		{
			const float aspectRatio = 1;
			const float farFlat = 10;
			const float nearFlat = 1;
			double halfFov = 90 * std::acos(-1.0) / 360.0;
			double cot = 1.0 / std::tan(halfFov);
			float flatDistence = 10.0 - 0.1;
			auto projection = glm::mat4(
				cot / aspectRatio, 0, 0, 0,
				0, cot, 0, 0,
				0, 0, -farFlat / flatDistence, -1,
				0, 0, -nearFlat * farFlat / flatDistence, 0
			);

			glm::vec3 rotation{};
			switch (i)
			{
				case RIGHT_INDEX:
				{
					rotation = {0, -90, 0};
					break;
				}
				case LEFT_INDEX:
				{
					rotation = { 0, 90, 0 };
					break;
				}
				case TOP_INDEX:
				{
					rotation = { -90, 0, 0 };
					break;
				}
				case BOTTOM_INDEX:
				{
					rotation = { 90, 0, 0 };
					break;
				}
				case FRONT_INDEX:
				{
					rotation = { 0, 180, 0 };
					break;
				}
				case BACK_INDEX:
				{
					rotation = { 0, 0, 0 };
					break;
				}
			}
			auto view = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), rotation.x, { 1, 0, 0 }), rotation.y, { 0, 1, 0 }), rotation.z, { 0, 0, 1 });

			splitInfo.viewProjection = projection * view;
		}

		commandBuffer->BeginRenderPass(_renderPass, featureData->_frameBuffers[i]);

		commandBuffer->PushConstant(featureData->_splitMaterial, VK_SHADER_STAGE_VERTEX_BIT, splitInfo);
		commandBuffer->DrawMesh(featureData->_cubeMesh, featureData->_splitMaterial);

		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::SplitHdrToCubeMap_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
