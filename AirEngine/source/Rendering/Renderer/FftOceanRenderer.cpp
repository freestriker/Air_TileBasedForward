#include "Rendering/Renderer/FftOceanRenderer.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include "Rendering/RenderFeature/ClearDepthAttachment_RenderFeature.h"
#include "Rendering/RenderFeature/FftOcean_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::FftOceanRenderer>("AirEngine::Rendering::Renderer::FftOceanRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::FftOceanRenderer::FftOceanRendererData>("AirEngine::Rendering::Renderer::FftOceanRenderer::FftOceanRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::FftOceanRenderer::FftOceanRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	_description = "This is a renderer for FFT based ocean, including FFT and Projected-Grid.";

	UseRenderFeature("ClearDepthAttachment_RenderFeature", new RenderFeature::ClearDepthAttachment_RenderFeature());

	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());

	UseRenderFeature("FftOcean_RenderFeature", new RenderFeature::FftOcean_RenderFeature());
}

AirEngine::Rendering::Renderer::FftOceanRenderer::~FftOceanRenderer()
{
	delete static_cast<RenderFeature::ClearDepthAttachment_RenderFeature*>(RenderFeature("ClearDepthAttachment_RenderFeature"));
	
	delete static_cast<RenderFeature::Background_RenderFeature*>(RenderFeature("Background_RenderFeature"));
	
	delete static_cast<RenderFeature::FftOcean_RenderFeature*>(RenderFeature("FftOcean_RenderFeature"));
}

AirEngine::Rendering::Renderer::FftOceanRenderer::FftOceanRendererData::FftOceanRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::FftOceanRenderer::FftOceanRendererData::~FftOceanRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::FftOceanRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new FftOceanRendererData();
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	rendererData->RenderFeatureData<RenderFeature::Background_RenderFeature::Background_RenderFeatureData>("Background_RenderFeature")->needClearColorAttachment = true;
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<FftOceanRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	PrepareRenderFeature("Background_RenderFeature", rendererData);

	PrepareRenderFeature("FftOcean_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("ClearDepthAttachment_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("FftOcean_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	SubmitRenderFeature("Background_RenderFeature", rendererData);

	SubmitRenderFeature("FftOcean_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::FftOceanRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	FinishRenderFeature("Background_RenderFeature", rendererData);

	FinishRenderFeature("FftOcean_RenderFeature", rendererData);
}
