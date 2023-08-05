#include "Rendering/RenderPipeline/BaseRenderPipeline.h"
#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
#include "Rendering/Renderer/ShadowVisualizationRenderer.h"
#include "Rendering/Renderer/BuildIblRenderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderPipeline::BaseRenderPipeline>("AirEngine::Rendering::RenderPipeline::BaseRenderPipeline")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderPipeline::BaseRenderPipeline::BaseRenderPipeline()
	: Core::Graphic::Rendering::RenderPipelineBase()
{
	UseRenderer("ForwardRenderer", new Renderer::ForwardRenderer());
	UseRenderer("TBForwardRenderer", new Renderer::TBForwardRenderer());
	UseRenderer("AmbientOcclusionRenderer", new Renderer::AmbientOcclusionRenderer());
	UseRenderer("ShadowVisualizationRenderer", new Renderer::ShadowVisualizationRenderer());
	UseRenderer("BuildIblRenderer", new Renderer::BuildIblRenderer());
}

AirEngine::Rendering::RenderPipeline::BaseRenderPipeline::~BaseRenderPipeline()
{
	delete static_cast<Renderer::ForwardRenderer*>(Renderer("ForwardRenderer"));
	delete static_cast<Renderer::TBForwardRenderer*>(Renderer("TBForwardRenderer"));
	delete static_cast<Renderer::ShadowVisualizationRenderer*>(Renderer("ShadowVisualizationRenderer"));
	delete static_cast<Renderer::BuildIblRenderer*>(Renderer("BuildIblRenderer"));
}
