#include "Rendering/RenderFeature/CascadeEVSM_ShadowCaster_RenderFeature.h"
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
#include "Core/Graphic/CoreObject/Instance.h"
#include <set>
#include "Light/LightBase.h"
#include "Utils/IntersectionChecker.h"
#include <array>
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include <algorithm>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderPass>("AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blit_RenderPass>("AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blit_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blur_RenderPass>("AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blur_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData>("AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature>("AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderPass::CascadeEVSM_ShadowCaster_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderPass::~CascadeEVSM_ShadowCaster_RenderPass()
{
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{},
		"DepthAttachment"
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
	creator.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
}
AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blit_RenderPass::CascadeEVSM_Blit_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blit_RenderPass::~CascadeEVSM_Blit_RenderPass()
{
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blit_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{"ColorAttachment"}
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
	creator.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
}
AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blur_RenderPass::CascadeEVSM_Blur_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blur_RenderPass::~CascadeEVSM_Blur_RenderPass()
{
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_Blur_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ShadowTexture",
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ShadowTexture" }
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
	creator.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData::CascadeEVSM_ShadowCaster_RenderFeatureData()
	: RenderFeatureDataBase()
	, depthAttachemnts()
	, shadowCasterFrameBuffers()
	, lightCameraInfoBuffer(nullptr)
	, lightCameraInfoStagingBuffer(nullptr)
	, frustumSegmentScales()
	, lightCameraCompensationDistances()
	, shadowImageResolutions()
	, cascadeEvsmShadowReceiverInfoBuffer(nullptr)
	, blitInfoBuffers()
	, shadowTextures()
	, blitFrameBuffers()
	, blitRenderPass(nullptr)
	, blitMaterials()
	, blurInfoBuffers()
	, blurMaterials()
	, blurFrameBuffers()
	, temporaryShadowTextures()
{
	frustumSegmentScales.fill(1.0 / CASCADE_COUNT);
	lightCameraCompensationDistances.fill(20);
	shadowImageResolutions.fill(1024);
	overlapScale = 0.3;
	c1 = 40;
	c2 = 5;
	threshold = 0.0;
	iterateCount = 1;
	blurOffsets.fill(1);
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData::~CascadeEVSM_ShadowCaster_RenderFeatureData()
{
}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature()
	: RenderFeatureBase()
	, _shadowCasterRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<CascadeEVSM_ShadowCaster_RenderPass>())
	, _blitRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<CascadeEVSM_Blit_RenderPass>())
	, _blurRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<CascadeEVSM_Blur_RenderPass>())
	, _shadowCasterRenderPassName(rttr::type::get<CascadeEVSM_ShadowCaster_RenderPass>().get_name().to_string())
	, _pointSampler(
		new Core::Graphic::Instance::ImageSampler(
			VkFilter::VK_FILTER_NEAREST,
			VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
		)
	)
	, _blitShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\CascadeEVSM_Blit_Shader.shader"))
	, _blurShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\CascadeEVSM_Blur_Shader.shader"))
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
{

}

AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::~CascadeEVSM_ShadowCaster_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<CascadeEVSM_ShadowCaster_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\CascadeEVSM_Blit_Shader.shader");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\CascadeEVSM_Blur_Shader.shader");
	delete _pointSampler;
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData::Refresh()
{
	for (auto i = 0; i < CASCADE_COUNT; i++)
	{
		VkExtent2D extent = { shadowImageResolutions[i], shadowImageResolutions[i] };
		delete depthAttachemnts[i];
		delete shadowCasterFrameBuffers[i];
		depthAttachemnts[i] = Core::Graphic::Instance::Image::Create2DImage(
			extent,
			VkFormat::VK_FORMAT_D32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT
		);
		shadowCasterFrameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			shadowCasterRenderPass,
			{
				{"DepthAttachment", depthAttachemnts[i]},
			}
		);

		delete shadowTextures[i];
		shadowTextures[i] = Core::Graphic::Instance::Image::Create2DImage(
			extent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);

		delete blitFrameBuffers[i];
		blitFrameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			blitRenderPass,
			{
				{"ColorAttachment", shadowTextures[i]},
			}
		);
		blitMaterials[i]->SetSampledImage2D("depthTexture", depthAttachemnts[i], pointSampler);
	
		delete temporaryShadowTextures[i];
		temporaryShadowTextures[i] = Core::Graphic::Instance::Image::Create2DImage(
			extent,
			VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		delete blurFrameBuffers[i];
		blurFrameBuffers[i] = new Core::Graphic::Rendering::FrameBuffer(
			blurRenderPass,
			{
				{"ShadowTexture", temporaryShadowTextures[i]},
			}
		);
		delete blurFrameBuffers[i + CASCADE_COUNT];
		blurFrameBuffers[i + CASCADE_COUNT] = new Core::Graphic::Rendering::FrameBuffer(
			blurRenderPass,
			{
				{"ShadowTexture", shadowTextures[i]},
			}
		);
		blurMaterials[i]->SetSampledImage2D("shadowTexture", shadowTextures[i], pointSampler);
		blurMaterials[i + CASCADE_COUNT]->SetSampledImage2D("shadowTexture", temporaryShadowTextures[i], pointSampler);
	}
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData::SetShadowReceiverMaterialParameters(Core::Graphic::Rendering::Material* material)
{
	material->SetUniformBuffer("cascadeEvsmShadowReceiverInfo", cascadeEvsmShadowReceiverInfoBuffer);
	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		material->SetSampledImage2D("shadowTexture_" + std::to_string(i), shadowTextures[i], pointSampler);
	}
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new CascadeEVSM_ShadowCaster_RenderFeatureData();
	featureData->shadowCasterRenderPass = _shadowCasterRenderPass;
	featureData->blitRenderPass = _blitRenderPass;
	featureData->blurRenderPass = _blurRenderPass;
	featureData->pointSampler = _pointSampler;

	for (auto i = 0; i < CASCADE_COUNT; i++)
	{
		featureData->blitInfoBuffers[i] = new Core::Graphic::Instance::Buffer(
			sizeof(CascadeEvsmBlitInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		featureData->blitMaterials[i] = new Core::Graphic::Rendering::Material(_blitShader);
		featureData->blitMaterials[i]->SetUniformBuffer("cascadeEvsmBlitInfo", featureData->blitInfoBuffers[i]);

		featureData->blurInfoBuffers[i] = new Core::Graphic::Instance::Buffer(
			sizeof(CascadeEvsmBlurInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		featureData->blurInfoBuffers[i + CASCADE_COUNT] = new Core::Graphic::Instance::Buffer(
			sizeof(CascadeEvsmBlurInfo),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		featureData->blurMaterials[i] = new Core::Graphic::Rendering::Material(_blurShader);
		featureData->blurMaterials[i + CASCADE_COUNT] = new Core::Graphic::Rendering::Material(_blurShader);
		featureData->blurMaterials[i]->SetUniformBuffer("cascadeEvsmBlurInfo", featureData->blurInfoBuffers[i]);
		featureData->blurMaterials[i + CASCADE_COUNT]->SetUniformBuffer("cascadeEvsmBlurInfo", featureData->blurInfoBuffers[i + CASCADE_COUNT]);
	}

	featureData->lightCameraInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(LightCameraInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	featureData->lightCameraInfoStagingBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(LightCameraInfo) * CASCADE_COUNT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	featureData->cascadeEvsmShadowReceiverInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(CascadeEvsmShadowReceiverInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	return featureData;
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<CascadeEVSM_ShadowCaster_RenderFeatureData*>(renderFeatureData);
	featureData->Refresh();
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<CascadeEVSM_ShadowCaster_RenderFeatureData*>(renderFeatureData);
	delete featureData->lightCameraInfoBuffer;
	delete featureData->lightCameraInfoStagingBuffer;
	delete featureData->cascadeEvsmShadowReceiverInfoBuffer;
	for (auto i = 0; i < CASCADE_COUNT; i++)
	{
		delete featureData->depthAttachemnts[i];
		delete featureData->shadowCasterFrameBuffers[i];

		delete featureData->shadowTextures[i];
		delete featureData->blitInfoBuffers[i];
		delete featureData->blitFrameBuffers[i];
		delete featureData->blitMaterials[i];

		delete featureData->temporaryShadowTextures[i];
		delete featureData->blurMaterials[i];
		delete featureData->blurMaterials[i + CASCADE_COUNT];
		delete featureData->blurInfoBuffers[i];
		delete featureData->blurInfoBuffers[i + CASCADE_COUNT];
		delete featureData->blurFrameBuffers[i];
		delete featureData->blurFrameBuffers[i + CASCADE_COUNT];
		
	}
	delete featureData;
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{

}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<CascadeEVSM_ShadowCaster_RenderFeatureData*>(renderFeatureData);

	glm::vec3 angularPointVPositions[8]{};
	camera->GetAngularPointVPosition(angularPointVPositions);

	glm::vec3 subAngularPointVPositions[CASCADE_COUNT][8]{};
	{
		std::array<float, CASCADE_COUNT> frustumSplitRatio = featureData->frustumSegmentScales;
		for (int i = 1; i < CASCADE_COUNT; i++)
		{
			frustumSplitRatio[i] = frustumSplitRatio[i - 1] + featureData->frustumSegmentScales[i];
		}

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			if (i == 0)
			{
				subAngularPointVPositions[i][0] = angularPointVPositions[0];
				subAngularPointVPositions[i][1] = angularPointVPositions[1];
				subAngularPointVPositions[i][2] = angularPointVPositions[2];
				subAngularPointVPositions[i][3] = angularPointVPositions[3];
			}
			else
			{
				subAngularPointVPositions[i][0] = (1 - frustumSplitRatio[i - 1]) * angularPointVPositions[0] + frustumSplitRatio[i - 1] * angularPointVPositions[4];
				subAngularPointVPositions[i][1] = (1 - frustumSplitRatio[i - 1]) * angularPointVPositions[1] + frustumSplitRatio[i - 1] * angularPointVPositions[5];
				subAngularPointVPositions[i][2] = (1 - frustumSplitRatio[i - 1]) * angularPointVPositions[2] + frustumSplitRatio[i - 1] * angularPointVPositions[6];
				subAngularPointVPositions[i][3] = (1 - frustumSplitRatio[i - 1]) * angularPointVPositions[3] + frustumSplitRatio[i - 1] * angularPointVPositions[7];
			}

			if (i == CASCADE_COUNT - 1)
			{
				subAngularPointVPositions[i][4] = angularPointVPositions[4];
				subAngularPointVPositions[i][5] = angularPointVPositions[5];
				subAngularPointVPositions[i][6] = angularPointVPositions[6];
				subAngularPointVPositions[i][7] = angularPointVPositions[7];
			}
			else
			{
				subAngularPointVPositions[i][4] = (1 - frustumSplitRatio[i]) * angularPointVPositions[0] + frustumSplitRatio[i] * angularPointVPositions[4];
				subAngularPointVPositions[i][5] = (1 - frustumSplitRatio[i]) * angularPointVPositions[1] + frustumSplitRatio[i] * angularPointVPositions[5];
				subAngularPointVPositions[i][6] = (1 - frustumSplitRatio[i]) * angularPointVPositions[2] + frustumSplitRatio[i] * angularPointVPositions[6];
				subAngularPointVPositions[i][7] = (1 - frustumSplitRatio[i]) * angularPointVPositions[3] + frustumSplitRatio[i] * angularPointVPositions[7];
			}
		}

		for (int i = CASCADE_COUNT - 1; i > 0; i--)
		{
			subAngularPointVPositions[i][0] = featureData->overlapScale * subAngularPointVPositions[i - 1][0] + (1 - featureData->overlapScale) * subAngularPointVPositions[i - 1][4];
			subAngularPointVPositions[i][1] = featureData->overlapScale * subAngularPointVPositions[i - 1][1] + (1 - featureData->overlapScale) * subAngularPointVPositions[i - 1][5];
			subAngularPointVPositions[i][2] = featureData->overlapScale * subAngularPointVPositions[i - 1][2] + (1 - featureData->overlapScale) * subAngularPointVPositions[i - 1][6];
			subAngularPointVPositions[i][3] = featureData->overlapScale * subAngularPointVPositions[i - 1][3] + (1 - featureData->overlapScale) * subAngularPointVPositions[i - 1][7];
		}
	}

	glm::vec3 sphereCenterVPositions[CASCADE_COUNT]{};
	float sphereRadius[CASCADE_COUNT]{};
	{
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			float len = std::abs(subAngularPointVPositions[i][0].z - subAngularPointVPositions[i][4].z);
			float a2 = glm::dot(subAngularPointVPositions[i][0] - subAngularPointVPositions[i][2], subAngularPointVPositions[i][0] - subAngularPointVPositions[i][2]);
			float b2 = glm::dot(subAngularPointVPositions[i][4] - subAngularPointVPositions[i][6], subAngularPointVPositions[i][4] - subAngularPointVPositions[i][6]);
			float x = len / 2 - (a2 - b2) / 8 / len;

			sphereCenterVPositions[i] = glm::vec3(0, 0, subAngularPointVPositions[i][0].z - x);
			sphereRadius[i] = std::sqrt(a2 / 4 + x * x);
		}
	}

	glm::vec3 lightVPositions[CASCADE_COUNT]{};
	LightCameraInfo lightCameraInfos[CASCADE_COUNT]{};
	CascadeEvsmShadowReceiverInfo cascadeEvsmShadowReceiverInfo{};
	CascadeEvsmBlitInfo cascadeEvsmBlitInfo{};
	{
		auto light = Core::Graphic::CoreObject::Instance::LightManager().MainLight();
		glm::mat4 cameraV = camera->ViewMatrix();
		glm::mat4 lightM = light->GameObject()->transform.ModelMatrix();
		glm::vec3 lightWPosition = lightM * glm::vec4(0, 0, 0, 1);
		glm::vec3 lightWView = glm::normalize(glm::vec3(lightM * glm::vec4(0, 0, -1, 1)) - lightWPosition);
		glm::vec3 lightVPosition = cameraV * lightM * glm::vec4(0, 0, 0, 1);
		glm::vec3 lightVUp = glm::normalize(glm::vec3(cameraV * lightM * glm::vec4(0, 1, 0, 1)) - lightVPosition);
		glm::vec3 lightVRight = glm::normalize(glm::vec3(cameraV * lightM * glm::vec4(1, 0, 0, 1)) - lightVPosition);
		glm::vec3 lightVView = glm::normalize(glm::vec3(cameraV * lightM * glm::vec4(0, 0, -1, 1)) - lightVPosition);

		cascadeEvsmBlitInfo.c1 = featureData->c1;
		cascadeEvsmBlitInfo.c2 = featureData->c2;
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			//glm::mat4 matrixVC2VL = glm::lookAt({ 0, 0, 0 }, lightVView, lightVUp);
			//float unit = sphereRadius[i] * 2 / featureData->shadowImageResolutions[i];

			//glm::vec3 virtualCenterLVPosition = matrixVC2VL * glm::vec4(sphereCenterVPositions[i], 1);
			//virtualCenterLVPosition.x -= std::fmod(virtualCenterLVPosition.x, unit);
			//virtualCenterLVPosition.y -= std::fmod(virtualCenterLVPosition.y, unit);

			//sphereCenterVPositions[i] = glm::inverse(matrixVC2VL) * glm::vec4(virtualCenterLVPosition, 1);

			lightVPositions[i] = sphereCenterVPositions[i] - lightVView * (sphereRadius[i] + featureData->lightCameraCompensationDistances[i]);
		}
		cascadeEvsmShadowReceiverInfo.wLightDirection = lightWView;
		cascadeEvsmShadowReceiverInfo.vLightDirection = lightVView;
		cascadeEvsmShadowReceiverInfo.c1 = featureData->c1;
		cascadeEvsmShadowReceiverInfo.c2 = featureData->c2;
		cascadeEvsmShadowReceiverInfo.threshold = featureData->threshold;
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			float halfWidth = sphereRadius[i];
			float flatDistence = sphereRadius[i] + sphereRadius[i] + featureData->lightCameraCompensationDistances[i];
			lightCameraInfos[i].projection = glm::mat4(
				1.0 / halfWidth, 0, 0, 0,
				0, 1.0 / halfWidth, 0, 0,
				0, 0, -1.0 / flatDistence, 0,
				0, 0, 0, 1
			);

			auto matrixVC2VL = glm::lookAt(lightVPositions[i], sphereCenterVPositions[i], lightVUp);
			lightCameraInfos[i].view = matrixVC2VL * cameraV;

			lightCameraInfos[i].viewProjection = lightCameraInfos[i].projection * lightCameraInfos[i].view;

			cascadeEvsmShadowReceiverInfo.matrixVC2PL[i] = lightCameraInfos[i].projection * matrixVC2VL;

			cascadeEvsmShadowReceiverInfo.thresholdVZ[i * 2 + 0].x = subAngularPointVPositions[i][0].z;
			cascadeEvsmShadowReceiverInfo.thresholdVZ[i * 2 + 1].x = subAngularPointVPositions[i][4].z;

			cascadeEvsmShadowReceiverInfo.texelSize[i] = { 1.0f / featureData->shadowImageResolutions[i], 1.0f / featureData->shadowImageResolutions[i], 0, 0 };

			cascadeEvsmBlitInfo.texelSize = cascadeEvsmShadowReceiverInfo.texelSize[i];
			featureData->blitInfoBuffers[i]->WriteData(&cascadeEvsmBlitInfo, sizeof(CascadeEvsmBlitInfo));

			CascadeEvsmBlurInfo cascadeEvsmBlurInfo{};
			cascadeEvsmBlurInfo.texelSize = cascadeEvsmShadowReceiverInfo.texelSize[i];
			cascadeEvsmBlurInfo.sampleOffset = { featureData->blurOffsets[i], 0 };
			featureData->blurInfoBuffers[i]->WriteData(&cascadeEvsmBlurInfo, sizeof(CascadeEvsmBlurInfo));
			cascadeEvsmBlurInfo.sampleOffset = { 0, featureData->blurOffsets[i] };
			featureData->blurInfoBuffers[i + CASCADE_COUNT]->WriteData(&cascadeEvsmBlurInfo, sizeof(CascadeEvsmBlurInfo));
		}

		std::sort(cascadeEvsmShadowReceiverInfo.thresholdVZ, cascadeEvsmShadowReceiverInfo.thresholdVZ + CASCADE_COUNT * 2, [](glm::vec4 a, glm::vec4 b)->bool {return a.x > b.x; });

		featureData->lightCameraInfoStagingBuffer->WriteData(&lightCameraInfos, sizeof(LightCameraInfo) * CASCADE_COUNT);
		featureData->cascadeEvsmShadowReceiverInfoBuffer->WriteData(&cascadeEvsmShadowReceiverInfo, sizeof(CascadeEvsmShadowReceiverInfo));
	}

	AirEngine::Utils::IntersectionChecker checkers[CASCADE_COUNT];
	{
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			glm::vec4 planes[6] =
			{
				glm::vec4(-1, 0, 0, sphereRadius[i]),
				glm::vec4(1, 0, 0, sphereRadius[i]),
				glm::vec4(0, -1, 0, sphereRadius[i]),
				glm::vec4(0, 1, 0, sphereRadius[i]),
				glm::vec4(0, 0, -1, 0),
				glm::vec4(0, 0, 1, sphereRadius[i] * 2 + featureData->lightCameraCompensationDistances[i])
			};
			checkers[i].SetIntersectPlanes(planes, 6);
		}
	}

	struct RendererWrapper
	{
		Core::Graphic::Rendering::Material* material;
		Asset::Mesh* mesh;
	};
	std::vector< RendererWrapper> rendererWrappers[CASCADE_COUNT];
	for (const auto& rendererComponent : *rendererComponents)
	{
		auto material = rendererComponent->GetMaterial(_shadowCasterRenderPassName);
		if (material == nullptr) continue;

		material->SetUniformBuffer("lightCameraInfo", featureData->lightCameraInfoBuffer);
		material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			auto obbVertexes = rendererComponent->mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = lightCameraInfos[i].view * rendererComponent->GameObject()->transform.ModelMatrix();
			if (rendererComponent->enableFrustumCulling && !checkers[i].Check(obbVertexes.data(), obbVertexes.size(), mvMatrix))
			{
				continue;
			}
			rendererWrappers[i].push_back({ material , rendererComponent->mesh });
		}
	}

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		commandBuffer->CopyBuffer(featureData->lightCameraInfoStagingBuffer, sizeof(LightCameraInfo) * i, featureData->lightCameraInfoBuffer, 0, sizeof(LightCameraInfo));

		VkClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.0f;
		commandBuffer->BeginRenderPass(
			_shadowCasterRenderPass,
			featureData->shadowCasterFrameBuffers[i],
			{
				{"DepthAttachment", depthClearValue}
			}
		);

		for (const auto& wrapper : rendererWrappers[i])
		{
			commandBuffer->DrawMesh(wrapper.mesh, wrapper.material);
		}

		commandBuffer->EndRenderPass();
	}

	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		commandBuffer->BeginRenderPass(_blitRenderPass, featureData->blitFrameBuffers[i]);

		commandBuffer->DrawMesh(_fullScreenMesh, featureData->blitMaterials[i]);

		commandBuffer->EndRenderPass();
	}

	for (int ii = 0; ii < featureData->iterateCount; ii++)
	{
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			commandBuffer->BeginRenderPass(_blurRenderPass, featureData->blurFrameBuffers[i]);
			commandBuffer->DrawMesh(_fullScreenMesh, featureData->blurMaterials[i]);
			commandBuffer->EndRenderPass();

			commandBuffer->BeginRenderPass(_blurRenderPass, featureData->blurFrameBuffers[i + CASCADE_COUNT]);
			commandBuffer->DrawMesh(_fullScreenMesh, featureData->blurMaterials[i + CASCADE_COUNT]);
			commandBuffer->EndRenderPass();
		}
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
