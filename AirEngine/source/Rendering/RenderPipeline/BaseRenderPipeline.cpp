#include "Rendering/RenderPipeline/BaseRenderPipeline.h"
#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
#include "Rendering/Renderer/ScreenSpaceShadowVisualizationRenderer.h"
#include "Rendering/Renderer/BuildIblRenderer.h"
#include "Rendering/Renderer/FftOceanRenderer.h"

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
	_description = "This is a basic rendering pipeline.";

	//UseRenderer("ForwardRenderer", new Renderer::ForwardRenderer());
	UseRenderer("TBForwardRenderer", new Renderer::TBForwardRenderer());
	UseRenderer("AmbientOcclusionRenderer", new Renderer::AmbientOcclusionRenderer());
	UseRenderer("ScreenSpaceShadowVisualizationRenderer", new Renderer::ScreenSpaceShadowVisualizationRenderer());
	UseRenderer("FftOceanRenderer", new Renderer::FftOceanRenderer());
	UseRenderer("BuildIblRenderer", new Renderer::BuildIblRenderer());
}

AirEngine::Rendering::RenderPipeline::BaseRenderPipeline::~BaseRenderPipeline()
{
	//delete static_cast<Renderer::ForwardRenderer*>(Renderer("ForwardRenderer"));
	delete static_cast<Renderer::TBForwardRenderer*>(Renderer("TBForwardRenderer"));
	delete static_cast<Renderer::AmbientOcclusionRenderer*>(Renderer("AmbientOcclusionRenderer"));
	delete static_cast<Renderer::ScreenSpaceShadowVisualizationRenderer*>(Renderer("ScreenSpaceShadowVisualizationRenderer"));
	delete static_cast<Renderer::FftOceanRenderer*>(Renderer("FftOceanRenderer"));
	delete static_cast<Renderer::BuildIblRenderer*>(Renderer("BuildIblRenderer"));
}
