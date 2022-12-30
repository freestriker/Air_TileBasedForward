#include "Rendering/Renderer/BuildAssetRenderer.h"
#include "Rendering/RenderFeature/SplitHdrToCubeMap_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include "Rendering/RenderFeature/GenerateIrradianceMap_RenderFeature.h"
#include "Rendering/RenderFeature/GeneratePrefilteredMap_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::BuildAssetRenderer>("AirEngine::Rendering::Renderer::BuildAssetRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::BuildAssetRenderer::BuildAssetRendererData>("AirEngine::Rendering::Renderer::BuildAssetRenderer::BuildAssetRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::BuildAssetRenderer::BuildAssetRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
	//UseRenderFeature("SplitHdrToHdrCubeImage_RenderFeature", new RenderFeature::SplitHdrToCubeMap_RenderFeature());
	//UseRenderFeature("SplitHdrToBackgroundCubeImage_RenderFeature", new RenderFeature::SplitHdrToCubeMap_RenderFeature());
	//UseRenderFeature("GenerateIrradianceMap_RenderFeature", new RenderFeature::GenerateIrradianceMap_RenderFeature());
	UseRenderFeature("GeneratePrefilteredMap_RenderFeature", new RenderFeature::GeneratePrefilteredMap_RenderFeature());
}

AirEngine::Rendering::Renderer::BuildAssetRenderer::~BuildAssetRenderer()
{
	delete static_cast<RenderFeature::SplitHdrToCubeMap_RenderFeature*>(RenderFeature("SplitHdrToHdrCubeImage_RenderFeature"));
}

AirEngine::Rendering::Renderer::BuildAssetRenderer::BuildAssetRendererData::BuildAssetRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::BuildAssetRenderer::BuildAssetRendererData::~BuildAssetRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::BuildAssetRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new BuildAssetRendererData();
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	auto backgroundRenderFeature = static_cast<RenderFeature::Background_RenderFeature::Background_RenderFeatureData*>(rendererData->RenderFeatureData("Background_RenderFeature"));
	backgroundRenderFeature->needClearColorAttachment = true;
	
	//auto toHdrRenderFeature = static_cast<RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData*>(rendererData->RenderFeatureData("SplitHdrToHdrCubeImage_RenderFeature"));
	//toHdrRenderFeature->hdrTexturePath = "..\\Asset\\Texture\\WorkShop_Exr_EquirectangularImage.json";
	//toHdrRenderFeature->shaderPath = "..\\Asset\\Shader\\SplitHdrToHdrCubeImage_Shader.shader";
	//toHdrRenderFeature->resolution = { 512, 512 };
	//
	//auto toBackgroundRenderFeature = static_cast<RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData*>(rendererData->RenderFeatureData("SplitHdrToBackgroundCubeImage_RenderFeature"));
	//toBackgroundRenderFeature->hdrTexturePath = "..\\Asset\\Texture\\WorkShop_Exr_EquirectangularImage.json";
	//toBackgroundRenderFeature->shaderPath = "..\\Asset\\Shader\\SplitHdrToBackgroundCubeImage_Shader.shader";
	//toBackgroundRenderFeature->resolution = { 1024, 1024 };

	//auto irradianceRenderFeature = static_cast<RenderFeature::GenerateIrradianceMap_RenderFeature::GenerateIrradianceMap_RenderFeatureData*>(rendererData->RenderFeatureData("GenerateIrradianceMap_RenderFeature"));
	//irradianceRenderFeature->environmentImagePath = "..\\Asset\\Texture\\WorkShop_Exr_CubeImage.json";
	//irradianceRenderFeature->stepCount = 256 * 256 * 4 * 4 * 4;
	//irradianceRenderFeature->sliceCount = 256 * 4;
	//irradianceRenderFeature->resolution = { 128, 128 };

	auto irradianceRenderFeature = static_cast<RenderFeature::GeneratePrefilteredMap_RenderFeature::GeneratePrefilteredMap_RenderFeatureData*>(rendererData->RenderFeatureData("GeneratePrefilteredMap_RenderFeature"));
	irradianceRenderFeature->environmentImagePath = "..\\Asset\\Texture\\WorkShop_Exr_CubeImage.json";
	irradianceRenderFeature->stepCount = 256 * 256 * 4 * 2;
	irradianceRenderFeature->sliceCount = 256 * 4;
	irradianceRenderFeature->resolution = { 512, 512 };
	irradianceRenderFeature->roughnessLevelCount = 5;
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<BuildAssetRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	//PrepareRenderFeature("SplitHdrToHdrCubeImage_RenderFeature", rendererData);
	//PrepareRenderFeature("SplitHdrToBackgroundCubeImage_RenderFeature", rendererData);
	//PrepareRenderFeature("GenerateIrradianceMap_RenderFeature", rendererData);
	PrepareRenderFeature("GeneratePrefilteredMap_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	//ExcuteRenderFeature("SplitHdrToHdrCubeImage_RenderFeature", rendererData, camera, rendererComponents);
	//ExcuteRenderFeature("SplitHdrToBackgroundCubeImage_RenderFeature", rendererData, camera, rendererComponents);
	//ExcuteRenderFeature("GenerateIrradianceMap_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("GeneratePrefilteredMap_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	//SubmitRenderFeature("SplitHdrToHdrCubeImage_RenderFeature", rendererData);
	//SubmitRenderFeature("SplitHdrToBackgroundCubeImage_RenderFeature", rendererData);
	//SubmitRenderFeature("GenerateIrradianceMap_RenderFeature", rendererData);
	SubmitRenderFeature("GeneratePrefilteredMap_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	//FinishRenderFeature("SplitHdrToHdrCubeImage_RenderFeature", rendererData);
	//FinishRenderFeature("SplitHdrToBackgroundCubeImage_RenderFeature", rendererData);
	//FinishRenderFeature("GenerateIrradianceMap_RenderFeature", rendererData);
	FinishRenderFeature("GeneratePrefilteredMap_RenderFeature", rendererData);
}
