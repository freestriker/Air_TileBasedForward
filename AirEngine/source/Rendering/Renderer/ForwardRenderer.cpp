#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/RenderFeature/GeometryRenderFeature.h"

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

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::ForwardRenderer::OnCreateRendererData()
{
	return new ForwardRendererData();
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<ForwardRendererData*>(rendererData);
}
