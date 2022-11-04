#include "Rendering/Renderer/BuildAssetRenderer.h"
#include "Rendering/RenderFeature/SplitHdrToCubeMap_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"

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
	UseRenderFeature("SplitHdrToHdrCubeMap_RenderFeature", new RenderFeature::SplitHdrToCubeMap_RenderFeature());
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
}

AirEngine::Rendering::Renderer::BuildAssetRenderer::~BuildAssetRenderer()
{
	delete static_cast<RenderFeature::SplitHdrToCubeMap_RenderFeature*>(RenderFeature("SplitHdrToHdrCubeMap_RenderFeature"));
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
	auto splitRenderFeature = static_cast<RenderFeature::SplitHdrToCubeMap_RenderFeature::SplitHdrToCubeMap_RenderFeatureData*>(rendererData->RenderFeatureData("SplitHdrToHdrCubeMap_RenderFeature"));
	splitRenderFeature->hdrTexturePath = "..\\Asset\\Texture\\WorkShop.json";
	splitRenderFeature->shaderPath = "..\\Asset\\Shader\\SplitHdrToCubeMap_Shader.shader";
	splitRenderFeature->resolution = { 1024, 1024 };

	auto backgroundRenderFeature = static_cast<RenderFeature::Background_RenderFeature::Background_RenderFeatureData*>(rendererData->RenderFeatureData("Background_RenderFeature"));
	backgroundRenderFeature->needClearColorAttachment = true;
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<BuildAssetRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	PrepareRenderFeature("SplitHdrToHdrCubeMap_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("SplitHdrToHdrCubeMap_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	SubmitRenderFeature("SplitHdrToHdrCubeMap_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::BuildAssetRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	FinishRenderFeature("SplitHdrToHdrCubeMap_RenderFeature", rendererData);
}
