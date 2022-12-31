#include "Rendering/RenderFeature/GenerateLutMap_RenderFeature.h"
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

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Accumulation_RenderPass>("AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Accumulation_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Pack_RenderPass>("AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Pack_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Unpack_RenderPass>("AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Unpack_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_RenderFeatureData>("AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature>("AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Accumulation_RenderPass::GenerateLutMap_Accumulation_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Accumulation_RenderPass::~GenerateLutMap_Accumulation_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Accumulation_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"AccumulationAttachment",
		VK_FORMAT_R32G32_SFLOAT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "AccumulationAttachment" }
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

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Pack_RenderPass::GenerateLutMap_Pack_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Pack_RenderPass::~GenerateLutMap_Pack_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Pack_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"PackAttachment",
		VK_FORMAT_R8G8B8A8_UINT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "PackAttachment" }
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

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Unpack_RenderPass::GenerateLutMap_Unpack_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Unpack_RenderPass::~GenerateLutMap_Unpack_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_Unpack_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"UnpackAttachment",
		VK_FORMAT_R32G32_SFLOAT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "UnpackAttachment" }
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

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_RenderFeatureData::GenerateLutMap_RenderFeatureData()
	: RenderFeatureDataBase()
	, _accumulationFrameBuffer(nullptr)
	, _packFrameBuffer(nullptr)
	, _unpackFrameBuffer(nullptr)
	, _accumulationShader(nullptr)
	, _accumulationMaterial(nullptr)
	, _packShader(nullptr)
	, _packMaterial(nullptr)
	, _unpackShader(nullptr)
	, _unpackMaterial(nullptr)
	, _accumulationImage(nullptr)
	, _packImage(nullptr)
	, _unpackSourceImage(nullptr)
	, _unpackImage(nullptr)
	, _sampler(nullptr)
	, _planeMesh(nullptr)
	, _sliceIndex(0)
	, resolution({ 512, 512 })
	, stepCount(256 * 256)
	, sliceCount(256)
{

}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_RenderFeatureData::~GenerateLutMap_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::GenerateLutMap_RenderFeature()
	: RenderFeatureBase()
	, _accumulationRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GenerateLutMap_Accumulation_RenderPass>())
	, _packRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GenerateLutMap_Pack_RenderPass>())
	, _unpackRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GenerateLutMap_Unpack_RenderPass>())
{

}

AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::~GenerateLutMap_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GenerateLutMap_Accumulation_RenderPass>();
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GenerateLutMap_Pack_RenderPass>();
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GenerateLutMap_Unpack_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	return new GenerateLutMap_RenderFeatureData();
}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<GenerateLutMap_RenderFeatureData*>(renderFeatureData);

	featureData->_planeMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");

	featureData->_accumulationImage = Core::Graphic::Instance::Image::Create2DImage(
		featureData->resolution,
		VK_FORMAT_R32G32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL
	);
	featureData->_accumulationFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_accumulationRenderPass,
		{
			{"AccumulationAttachment", featureData->_accumulationImage}
		}
	);

	featureData->_packImage = Core::Graphic::Instance::Image::Create2DImage(
		featureData->resolution,
		VK_FORMAT_R8G8B8A8_UINT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL
	);
	featureData->_packFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_packRenderPass,
		{
			{"PackAttachment", featureData->_packImage}
		}
	);

	featureData->_unpackSourceImage = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\LutImage.json");
	featureData->_unpackImage = Core::Graphic::Instance::Image::Create2DImage(
		featureData->resolution,
		VK_FORMAT_R32G32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL
	);
	featureData->_unpackFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_unpackRenderPass,
		{
			{"UnpackAttachment", featureData->_unpackImage}
		}
	);

	featureData->_sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	featureData->_accumulationShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GenerateLutMap_Accumulation_Shader.shader");
	featureData->_accumulationMaterial = new Core::Graphic::Rendering::Material(featureData->_accumulationShader);

	featureData->_packShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GenerateLutMap_Pack_Shader.shader");
	featureData->_packMaterial = new Core::Graphic::Rendering::Material(featureData->_packShader);
	featureData->_packMaterial->SetSampledImage2D("accumulationImage", featureData->_accumulationImage, featureData->_sampler);

	featureData->_unpackShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GenerateLutMap_Unpack_Shader.shader");
	featureData->_unpackMaterial = new Core::Graphic::Rendering::Material(featureData->_unpackShader);
	featureData->_unpackMaterial->SetSampledImage2D("packImage", featureData->_unpackSourceImage, featureData->_sampler);
}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GenerateLutMap_RenderFeatureData*>(renderFeatureData);

	delete featureData->_accumulationFrameBuffer;
	delete featureData->_packFrameBuffer;
	delete featureData->_unpackFrameBuffer;

	delete featureData->_accumulationMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_accumulationShader);

	delete featureData->_packMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_packShader);

	delete featureData->_unpackMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_unpackShader);

	delete featureData->_accumulationImage;
	delete featureData->_packImage;
	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_unpackSourceImage);
	delete featureData->_unpackImage;
	delete featureData->_sampler;

	Core::IO::CoreObject::Instance::AssetManager().Unload(featureData->_planeMesh);
}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<GenerateLutMap_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (featureData->_sliceIndex == 0)
	{
		///Clear image
		{
			{
				auto accumulationImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_accumulationImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &accumulationImageBarrier }
				);
			}

			commandBuffer->ClearColorImage(featureData->_accumulationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { 0.0f, 0.0f, 0.0f, 0.0f });

			{
				auto accumulationImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_accumulationImage,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
				);
				auto packImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_packImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_NONE
				); 
				auto unpackImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_unpackImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_NONE,
					VkAccessFlagBits::VK_ACCESS_NONE
				); 
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					{ &accumulationImageBarrier, &packImageBarrier, &unpackImageBarrier }
				);
			}
		}
	}

	if (featureData->_sliceIndex < featureData->sliceCount)
	{
		const AccumulationPushConstantInfo accumulationInfo
		{
			featureData->stepCount,
			featureData->sliceCount,
			featureData->_sliceIndex,
			featureData->resolution.width
		};

		commandBuffer->BeginRenderPass(_accumulationRenderPass, featureData->_accumulationFrameBuffer);
		commandBuffer->PushConstant(featureData->_accumulationMaterial, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, accumulationInfo);
		commandBuffer->DrawMesh(featureData->_planeMesh, featureData->_accumulationMaterial);
		commandBuffer->EndRenderPass();

		if (featureData->_sliceIndex == featureData->sliceCount - 1)
		{
			{
				auto accumulationImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->_accumulationImage,
					VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VkAccessFlagBits::VK_ACCESS_NONE
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					{ &accumulationImageBarrier }
				);
			}
		}
	}

	if (featureData->_sliceIndex >= featureData->sliceCount)
	{
		const PackPushConstantInfo packInfo
		{
			featureData->stepCount,
			featureData->resolution.width
		};

		commandBuffer->BeginRenderPass(_packRenderPass, featureData->_packFrameBuffer);
		commandBuffer->PushConstant(featureData->_packMaterial, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, packInfo);
		commandBuffer->DrawMesh(featureData->_planeMesh, featureData->_packMaterial);
		commandBuffer->EndRenderPass();
	}

	{
		const PackPushConstantInfo unpackInfo
		{
			featureData->stepCount,
			featureData->resolution.width
		};

		commandBuffer->BeginRenderPass(_unpackRenderPass, featureData->_unpackFrameBuffer);
		commandBuffer->PushConstant(featureData->_unpackMaterial, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, unpackInfo);
		commandBuffer->DrawMesh(featureData->_planeMesh, featureData->_unpackMaterial);
		commandBuffer->EndRenderPass();
	}

	commandBuffer->EndRecord();

	if (featureData->_sliceIndex > featureData->sliceCount)
	{
		Utils::Log::Message("Generating lut map is completed.");
	}
	else
	{
		featureData->_sliceIndex++;
	}
}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::GenerateLutMap_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
