#include "Rendering/Renderer/OceanRenderer.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include "Rendering/RenderFeature/ClearDepthAttachment_RenderFeature.h"
#include "Rendering/RenderFeature/FftOcean_RenderFeature.h"
#include "Rendering/RenderFeature/GerstnerOcean_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::OceanRenderer>("AirEngine::Rendering::Renderer::OceanRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::OceanRenderer::OceanRendererData>("AirEngine::Rendering::Renderer::FftOceanRenderer::OceanRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::OceanRenderer::OceanRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	_description = "This is a renderer for ocean.";

	UseRenderFeature("ClearDepthAttachment_RenderFeature", new RenderFeature::ClearDepthAttachment_RenderFeature());

	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());

	UseRenderFeature("FftOcean_RenderFeature", new RenderFeature::FftOcean_RenderFeature());
	UseRenderFeature("GerstnerOcean_RenderFeature", new RenderFeature::GerstnerOcean_RenderFeature());
}

AirEngine::Rendering::Renderer::OceanRenderer::~OceanRenderer()
{
	delete static_cast<RenderFeature::ClearDepthAttachment_RenderFeature*>(RenderFeature("ClearDepthAttachment_RenderFeature"));
	
	delete static_cast<RenderFeature::Background_RenderFeature*>(RenderFeature("Background_RenderFeature"));
	
	delete static_cast<RenderFeature::FftOcean_RenderFeature*>(RenderFeature("FftOcean_RenderFeature"));
	delete static_cast<RenderFeature::GerstnerOcean_RenderFeature*>(RenderFeature("GerstnerOcean_RenderFeature"));
}

AirEngine::Rendering::Renderer::OceanRenderer::OceanRendererData::OceanRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::OceanRenderer::OceanRendererData::~OceanRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::OceanRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new OceanRendererData();
}

void AirEngine::Rendering::Renderer::OceanRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	rendererData->RenderFeatureData<RenderFeature::Background_RenderFeature::Background_RenderFeatureData>("Background_RenderFeature")->needClearColorAttachment = true;
}

void AirEngine::Rendering::Renderer::OceanRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<OceanRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::OceanRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	PrepareRenderFeature("Background_RenderFeature", rendererData);

	PrepareRenderFeature("FftOcean_RenderFeature", rendererData);
	PrepareRenderFeature("GerstnerOcean_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::OceanRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("ClearDepthAttachment_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("FftOcean_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("GerstnerOcean_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::OceanRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	SubmitRenderFeature("Background_RenderFeature", rendererData);

	SubmitRenderFeature("FftOcean_RenderFeature", rendererData);
	SubmitRenderFeature("GerstnerOcean_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::OceanRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("ClearDepthAttachment_RenderFeature", rendererData);

	FinishRenderFeature("Background_RenderFeature", rendererData);

	FinishRenderFeature("FftOcean_RenderFeature", rendererData);
	FinishRenderFeature("GerstnerOcean_RenderFeature", rendererData);
}
