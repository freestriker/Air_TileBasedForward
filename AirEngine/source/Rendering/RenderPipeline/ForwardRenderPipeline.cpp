#include "Rendering/RenderPipeline/ForwardRenderPipeline.h"
#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
#include "Rendering/Renderer/ShadowVisualizationRenderer.h"
#include "Rendering/Renderer/BuildAssetRenderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderPipeline::ForwardRenderPipeline>("AirEngine::Rendering::RenderPipeline::ForwardRenderPipeline")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderPipeline::ForwardRenderPipeline::ForwardRenderPipeline()
	: Core::Graphic::Rendering::RenderPipelineBase()
{
	UseRenderer("ForwardRenderer", new Renderer::ForwardRenderer());
	UseRenderer("TBForwardRenderer", new Renderer::TBForwardRenderer());
	UseRenderer("AmbientOcclusionRenderer", new Renderer::AmbientOcclusionRenderer());
	UseRenderer("ShadowVisualizationRenderer", new Renderer::ShadowVisualizationRenderer());
	UseRenderer("BuildAssetRenderer", new Renderer::BuildAssetRenderer());
}

AirEngine::Rendering::RenderPipeline::ForwardRenderPipeline::~ForwardRenderPipeline()
{
	delete static_cast<Renderer::ForwardRenderer*>(Renderer("ForwardRenderer"));
	delete static_cast<Renderer::TBForwardRenderer*>(Renderer("TBForwardRenderer"));
	delete static_cast<Renderer::ShadowVisualizationRenderer*>(Renderer("ShadowVisualizationRenderer"));
	delete static_cast<Renderer::BuildAssetRenderer*>(Renderer("BuildAssetRenderer"));
}
