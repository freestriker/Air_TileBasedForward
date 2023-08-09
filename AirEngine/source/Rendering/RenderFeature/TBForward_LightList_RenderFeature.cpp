#include "Rendering/RenderFeature/TBForward_LightList_RenderFeature.h"
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
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData>("AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature>("AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData::TBForward_LightList_RenderFeatureData()
	: RenderFeatureDataBase()
	, material(nullptr)
	, depthTextureInfoBuffer(nullptr)
	, opaqueLightIndexListsBuffer(nullptr)
	, transparentLightIndexListsBuffer(nullptr)
	, depthTexture(nullptr)
{

}

AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData::~TBForward_LightList_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeature()
	: RenderFeatureBase()
	, _shader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_LightList_Shader.shader"))
	, _depthTextureSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
{
	_description = "Use compute shader to generate the light index tables used in opaque rendering and transparent rendering.";
}

AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::~TBForward_LightList_RenderFeature()
{
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\TBF_LightList_Shader.shader");
	delete _depthTextureSampler;
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new TBForward_LightList_RenderFeatureData();

	auto depthTextureSize = camera->attachments["DepthAttachment"]->VkExtent2D_();

	VkExtent2D globalGroupSize = { (depthTextureSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (depthTextureSize.height + TILE_WIDTH - 1) / TILE_WIDTH };
	featureData->opaqueLightIndexListsBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(LightIndexList) * globalGroupSize.width * globalGroupSize.height + 8,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	featureData->transparentLightIndexListsBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(LightIndexList) * globalGroupSize.width * globalGroupSize.height + 8,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	featureData->depthTextureInfoBuffer = new Core::Graphic::Instance::Buffer{
		sizeof(DepthTextureInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	auto depthTextureInfo = DepthTextureInfo{ glm::ivec2(depthTextureSize.width, depthTextureSize.height) , glm::vec2(1, 1) / glm::vec2(depthTextureSize.width, depthTextureSize.height) };
	featureData->depthTextureInfoBuffer->WriteData(&depthTextureInfo, sizeof(DepthTextureInfo));

	return featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<TBForward_LightList_RenderFeatureData*>(renderFeatureData);

	featureData->material = new Core::Graphic::Rendering::Material(_shader);
	featureData->material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	featureData->material->SetUniformBuffer("lightBoundingBoxInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightBoundindBoxInfosBuffer());
	featureData->material->SetStorageBuffer("opaqueLightIndexLists", featureData->opaqueLightIndexListsBuffer);
	featureData->material->SetStorageBuffer("transparentLightIndexLists", featureData->transparentLightIndexListsBuffer);
	featureData->material->SetSampledImage2D("depthTexture", featureData->depthTexture, _depthTextureSampler);
	featureData->material->SetUniformBuffer("depthTextureInfo", featureData->depthTextureInfoBuffer);
}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<TBForward_LightList_RenderFeatureData*>(renderFeatureData);

	delete featureData->material;
	delete featureData->depthTextureInfoBuffer;
	delete featureData->opaqueLightIndexListsBuffer;
	delete featureData->transparentLightIndexListsBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<TBForward_LightList_RenderFeatureData*>(renderFeatureData);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Clear lists buffer
	commandBuffer->FillBuffer(featureData->opaqueLightIndexListsBuffer, 0);
	commandBuffer->FillBuffer(featureData->transparentLightIndexListsBuffer, 0);

	///Wait clear lists buffer finish
	{
		Core::Graphic::Command::BufferMemoryBarrier opaqueLightListsBarrier = Core::Graphic::Command::BufferMemoryBarrier
		(
			featureData->opaqueLightIndexListsBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
		);
		Core::Graphic::Command::BufferMemoryBarrier transparentLightListsBarrier = Core::Graphic::Command::BufferMemoryBarrier
		(
			featureData->transparentLightIndexListsBuffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
		);
		commandBuffer->AddPipelineBufferBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			{ &opaqueLightListsBarrier, &transparentLightListsBarrier }
		);
	}
	auto depthTextureSize = featureData->depthTexture->VkExtent2D_();

	VkExtent2D globalGroupSize = { (depthTextureSize.width + TILE_WIDTH - 1) / TILE_WIDTH, (depthTextureSize.height + TILE_WIDTH - 1) / TILE_WIDTH };

	commandBuffer->Dispatch(featureData->material, globalGroupSize.width, globalGroupSize.height, 1);

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::TBForward_LightList_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
