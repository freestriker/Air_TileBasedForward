#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/RenderFeature/GeometryRenderFeature.h"
#include "Rendering/RenderFeature/Forward_Opaque_RenderFeature.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include "Rendering/RenderFeature/Forward_Transparent_RenderFeature.h"


RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::ForwardRenderer>("AirEngine::Rendering::Renderer::ForwardRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData>("AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	UseRenderFeature("GeometryRenderFeature", new RenderFeature::GeometryRenderFeature());
	UseRenderFeature("Forward_Opaque_RenderFeature", new RenderFeature::Forward_Opaque_RenderFeature());
	UseRenderFeature("Forward_Transparent_RenderFeature", new RenderFeature::Forward_Transparent_RenderFeature());
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
}

AirEngine::Rendering::Renderer::ForwardRenderer::~ForwardRenderer()
{
	delete static_cast<RenderFeature::GeometryRenderFeature*>(RenderFeature("GeometryRenderFeature"));
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData::ForwardRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData::~ForwardRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::ForwardRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new ForwardRendererData();
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	static_cast<RenderFeature::Background_RenderFeature::Background_RenderFeatureData*>(rendererData->RenderFeatureData("Background_RenderFeature"))->needClearColorAttachment = true;
	static_cast<RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData*>(rendererData->RenderFeatureData("Forward_Opaque_RenderFeature"))->needClearColorAttachment = false;
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<ForwardRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	PrepareRenderFeature("GeometryRenderFeature", rendererData);
	PrepareRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	PrepareRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("GeometryRenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Forward_Opaque_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Forward_Transparent_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	SubmitRenderFeature("GeometryRenderFeature", rendererData);
	SubmitRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	SubmitRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	FinishRenderFeature("GeometryRenderFeature", rendererData);
	FinishRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	FinishRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}
 