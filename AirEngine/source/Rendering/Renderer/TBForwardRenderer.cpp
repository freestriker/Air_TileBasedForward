#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/RenderFeature/GeometryRenderFeature.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/TBForward_LightList_RenderFeature.h"
#include "Rendering/RenderFeature/TBForward_Opaque_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::TBForwardRenderer>("AirEngine::Rendering::Renderer::TBForwardRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::TBForwardRenderer::TBForwardRendererData>("AirEngine::Rendering::Renderer::TBForwardRenderer::TBForwardRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::TBForwardRenderer::TBForwardRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	UseRenderFeature("GeometryRenderFeature", new RenderFeature::GeometryRenderFeature());
	UseRenderFeature("TBForward_LightList_RenderFeature", new RenderFeature::TBForward_LightList_RenderFeature());
	UseRenderFeature("TBForward_Opaque_RenderFeature", new RenderFeature::TBForward_Opaque_RenderFeature());
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
}

AirEngine::Rendering::Renderer::TBForwardRenderer::~TBForwardRenderer()
{
	delete static_cast<RenderFeature::GeometryRenderFeature*>(RenderFeature("GeometryRenderFeature"));
}

AirEngine::Rendering::Renderer::TBForwardRenderer::TBForwardRendererData::TBForwardRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::TBForwardRenderer::TBForwardRendererData::~TBForwardRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::TBForwardRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new TBForwardRendererData();
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	static_cast<RenderFeature::Background_RenderFeature::Background_RenderFeatureData*>(rendererData->RenderFeatureData("Background_RenderFeature"))->needClearColorAttachment = true;
	auto geometryFeatureData = static_cast<RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData*>(rendererData->RenderFeatureData("GeometryRenderFeature"));
	auto lightListFeatureData = static_cast<RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData*>(rendererData->RenderFeatureData("TBForward_LightList_RenderFeature"));
	auto opaqueFeatureData = static_cast<RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData*>(rendererData->RenderFeatureData("TBForward_Opaque_RenderFeature"));
	lightListFeatureData->depthTexture = geometryFeatureData->depthTexture;
	opaqueFeatureData->opaqueLightIndexListsBuffer = lightListFeatureData->opaqueLightIndexListsBuffer;
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<TBForwardRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	PrepareRenderFeature("GeometryRenderFeature", rendererData);
	PrepareRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	PrepareRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("GeometryRenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("TBForward_LightList_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("TBForward_Opaque_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	SubmitRenderFeature("GeometryRenderFeature", rendererData);
	SubmitRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	SubmitRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	FinishRenderFeature("GeometryRenderFeature", rendererData);
	FinishRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	FinishRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
}
