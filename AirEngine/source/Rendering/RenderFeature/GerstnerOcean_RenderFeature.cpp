#include "Rendering/RenderFeature/GerstnerOcean_RenderFeature.h"
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
#include <Rendering/Utility/ProjectedGridUtility.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::GerstnerOcean_Surface_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::~GerstnerOcean_Surface_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
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
	//settings.AddDependency(
	//	"VK_SUBPASS_EXTERNAL",
	//	"DrawSubpass",
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	//);
	//settings.AddDependency(
	//	"DrawSubpass",
	//	"VK_SUBPASS_EXTERNAL",
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	//);
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData::GerstnerOcean_RenderFeatureData()
	: RenderFeatureDataBase()
{

}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData::~GerstnerOcean_RenderFeatureData()
{

}
AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GerstnerOcean_Surface_RenderPass>())
	, _renderPassName(rttr::type::get<GerstnerOcean_Surface_RenderPass>().get_name().to_string())
{
	_description = "Use Gerstner and Projected-Grid to render ocean.";
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::~GerstnerOcean_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GerstnerOcean_Surface_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	GerstnerOcean_RenderFeatureData* featureDataPtr = new GerstnerOcean_RenderFeatureData();
	GerstnerOcean_RenderFeatureData& featureData = *featureDataPtr;

	featureData.isDirty = true;

	featureData.subGerstnerWaveInfos.reserve(5);
	featureData.subGerstnerWaveInfos.resize(1);
	featureData.subGerstnerWaveInfos.at(0).amplitudeFactor = 1;
	featureData.subGerstnerWaveInfos.at(0).windRotationAngle = 0;
	featureData.subGerstnerWaveInfos.at(0).waveLength = 1;
	featureData.subGerstnerWaveInfos.at(0).omegaFactor = 1;
	featureData.subGerstnerWaveInfos.at(0).phiAngle = 0;

	featureData.displacementFactor = { 1, 1, 1 };
	featureData.oceanScale = { 100, 100, 100 };
	featureData.absDisplacement = glm::vec3(0.12, 0.12, 0.12);
	featureData.aimPointDistanceFactor = 10;
	featureData.aimPointHeightCompensation = 20;
	featureData.showWireFrame = false;

	featureData.widgetLauncher = new GerstnerOceanDataWidgetLauncher(featureData);
	featureData.widgetLauncher->Launch();

	featureData.frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);

	featureData.gerstnerWaveInfoStagingBuffer = nullptr;

	featureData.gerstnerWaveInfoBuffer = nullptr;

	//featureData.gerstnerWaveInfoStagingBuffer = new Core::Graphic::Instance::Buffer(
	//	sizeof(GerstnerOcean_RenderFeatureData::SubGerstnerWaveInfo) * featureData.subGerstnerWaveInfos.size(),
	//	VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//);

	//featureData.gerstnerWaveInfoBuffer = new Core::Graphic::Instance::Buffer(
	//	sizeof(GerstnerOcean_RenderFeatureData::SubGerstnerWaveInfo) * featureData.subGerstnerWaveInfos.size(),
	//	VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
	//	VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	//);

	featureData.surfaceMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Surface.ply");
	featureData.surfaceShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GerstnerOcean_Surface_Shader.shader");
	featureData.surfaceMaterial = new Core::Graphic::Rendering::Material(featureData.surfaceShader);
	featureData.surfaceMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	featureData.surfaceMaterial->SetStorageBuffer("gerstnerWaveInfoBuffer", featureData.gerstnerWaveInfoBuffer);

	featureData.surfaceWireFrameShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GerstnerOcean_SurfaceWireFrame_Shader.shader");
	featureData.surfaceWireFrameMaterial = new Core::Graphic::Rendering::Material(featureData.surfaceWireFrameShader);
	featureData.surfaceWireFrameMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	featureData.surfaceMaterial->SetStorageBuffer("gerstnerWaveInfoBuffer", featureData.gerstnerWaveInfoBuffer);

	return featureDataPtr;
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GerstnerOcean_RenderFeatureData*>(renderFeatureData);

	featureData->widgetLauncher->Terminate();

	delete featureData->frameBuffer;

	delete featureData->surfaceMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\GerstnerOcean_Surface_Shader.shader");
	delete featureData->surfaceWireFrameMaterial;
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\GerstnerOcean_SurfaceWireFrame_Shader.shader");

	delete featureData->gerstnerWaveInfoStagingBuffer;
	delete featureData->gerstnerWaveInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOceanDataWidgetLauncher::OnSetUp()
{
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOceanDataWidgetLauncher::GerstnerOceanDataWidgetLauncher(GerstnerOcean_RenderFeatureData& data)
	: AirEngine::Utils::DataWidgetLauncher< GerstnerOcean_RenderFeatureData>(data)
{

}
