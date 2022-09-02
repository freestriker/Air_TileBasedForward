#include "Core/Graphic/Rendering/RenderPipelineBase.h"
#include "Core/Graphic/Rendering/RendererBase.h"
#include "Utils/Log.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RenderPipelineBase>("AirEngine::Core::Graphic::Rendering::RenderPipelineBase");
}

AirEngine::Core::Graphic::Rendering::RenderPipelineBase::RenderPipelineBase()
{
}

AirEngine::Core::Graphic::Rendering::RenderPipelineBase::~RenderPipelineBase()
{
}

void AirEngine::Core::Graphic::Rendering::RenderPipelineBase::UseRenderer(std::string rendererName, RendererBase* renderer)
{
	Utils::Log::Exception("Already contains renderer named: " + rendererName + ".", _renderers.count(rendererName) != 0);
	_renderers[rendererName] = renderer;
}

AirEngine::Core::Graphic::Rendering::RendererBase* AirEngine::Core::Graphic::Rendering::RenderPipelineBase::Renderer(std::string rendererName)
{
	return _renderers[rendererName];
}