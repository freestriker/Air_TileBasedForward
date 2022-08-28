#include "Rendering/RenderPipeline/ForwardRenderPipeline.h"
#include "Rendering/Renderer/ForwardRenderer.h"

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
}

AirEngine::Rendering::RenderPipeline::ForwardRenderPipeline::~ForwardRenderPipeline()
{
	delete static_cast<Renderer::ForwardRenderer*>(Renderer("ForwardRenderer"));
}
