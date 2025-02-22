#include "Rendering/Renderer/ScreenSpaceShadowVisualizationRenderer.h"
#include "Rendering/RenderFeature/Geometry_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/ClearColorAttachment_RenderFeature.h"
#include "Rendering/RenderFeature/CSM_ShadowCaster_RenderFeature.h"
#include "Rendering/RenderFeature/CascadeEVSM_ShadowCaster_RenderFeature.h"
#include "Rendering/RenderFeature/CSM_Visualization_RenderFeature.h"
#include "Rendering/RenderFeature/CascadeEVSM_Visualization_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer>("AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRendererData>("AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	_description = "This is a renderer for visualizing cascading shadows in screen space, including different cascading shadows with exponential variance cascading shadows.";

	UseRenderFeature("Geometry_RenderFeature", new RenderFeature::Geometry_RenderFeature());
	UseRenderFeature("ClearColorAttachment_RenderFeature", new RenderFeature::ClearColorAttachment_RenderFeature());

	UseRenderFeature("CSM_ShadowCaster_RenderFeature", new RenderFeature::CSM_ShadowCaster_RenderFeature());
	UseRenderFeature("CSM_Visualization_RenderFeature", new RenderFeature::CSM_Visualization_RenderFeature());
	
	UseRenderFeature("CascadeEVSM_ShadowCaster_RenderFeature", new RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature());
	UseRenderFeature("CascadeEVSM_Visualization_RenderFeature", new RenderFeature::CascadeEVSM_Visualization_RenderFeature());
}

AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::~ScreenSpaceShadowVisualizationRenderer()
{
	delete static_cast<RenderFeature::Geometry_RenderFeature*>(RenderFeature("Geometry_RenderFeature"));
	delete static_cast<RenderFeature::ClearColorAttachment_RenderFeature*>(RenderFeature("ClearColorAttachment_RenderFeature"));
	
	delete static_cast<RenderFeature::CSM_ShadowCaster_RenderFeature*>(RenderFeature("CSM_ShadowCaster_RenderFeature"));
	delete static_cast<RenderFeature::CSM_Visualization_RenderFeature*>(RenderFeature("CSM_Visualization_RenderFeature"));
	
	delete static_cast<RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature*>(RenderFeature("CascadeEVSM_ShadowCaster_RenderFeature"));
	delete static_cast<RenderFeature::CascadeEVSM_Visualization_RenderFeature*>(RenderFeature("CascadeEVSM_Visualization_RenderFeature"));
}

AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRendererData::ScreenSpaceShadowVisualizationRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
	, shadowType(ShadowType::CSM)
{
}

AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRendererData::~ScreenSpaceShadowVisualizationRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new ScreenSpaceShadowVisualizationRendererData();
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	auto clearFeatureData = rendererData->RenderFeatureData<RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData>("ClearColorAttachment_RenderFeature");
	clearFeatureData->clearValue = { 255, 255, 255, 255 };
	
	auto geometryFeatureData = rendererData->RenderFeatureData<RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData>("Geometry_RenderFeature");
	
	auto csmShadowMapFeatureData = rendererData->RenderFeatureData<RenderFeature::CSM_ShadowCaster_RenderFeature::CSM_ShadowCaster_RenderFeatureData>("CSM_ShadowCaster_RenderFeature");
	csmShadowMapFeatureData->frustumSegmentScales = { 0.1, 0.2, 0.3, 0.4 };
	csmShadowMapFeatureData->lightCameraCompensationDistances = { 5, 5, 5, 5 };
	csmShadowMapFeatureData->shadowImageResolutions = 2048;
	csmShadowMapFeatureData->sampleHalfWidth = 2;
	csmShadowMapFeatureData->bias = {
		glm::vec2{ 0.0020f, 0.0075f },
		glm::vec2{ 0.0020f, 0.0095f },
		glm::vec2{ 0.0025f, 0.0105f },
		glm::vec2{ 0.0025f, 0.0115f },
	};

	auto csmVisualizationFeatureData = rendererData->RenderFeatureData<RenderFeature::CSM_Visualization_RenderFeature::CSM_Visualization_RenderFeatureData>("CSM_Visualization_RenderFeature");
	csmVisualizationFeatureData->csmRenderFeatureData = csmShadowMapFeatureData;
	csmVisualizationFeatureData->geometryRenderFeatureData = geometryFeatureData;

	auto cevsmShadowMapFeatureData = rendererData->RenderFeatureData<RenderFeature::CascadeEVSM_ShadowCaster_RenderFeature::CascadeEVSM_ShadowCaster_RenderFeatureData>("CascadeEVSM_ShadowCaster_RenderFeature");
	cevsmShadowMapFeatureData->frustumSegmentScales = { 0.1, 0.2, 0.3, 0.4 };
	cevsmShadowMapFeatureData->lightCameraCompensationDistances = { 5, 5, 5, 5 };
	cevsmShadowMapFeatureData->shadowImageResolutions = { 2048, 2048, 1024, 1024 };
	cevsmShadowMapFeatureData->blurOffsets = { 1.5, 1.25, 1.0, 0.5 };
	cevsmShadowMapFeatureData->iterateCount = 2;
	cevsmShadowMapFeatureData->threshold = 0.5;

	auto cevsmVisualizationFeatureData = rendererData->RenderFeatureData<RenderFeature::CascadeEVSM_Visualization_RenderFeature::CascadeEVSM_Visualization_RenderFeatureData>("CascadeEVSM_Visualization_RenderFeature");
	cevsmVisualizationFeatureData->cascadeEvsmRenderFeatureData = cevsmShadowMapFeatureData;
	cevsmVisualizationFeatureData->geometryRenderFeatureData = geometryFeatureData;
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<ScreenSpaceShadowVisualizationRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	PrepareRenderFeature("Geometry_RenderFeature", rendererData);

	switch (static_cast<ScreenSpaceShadowVisualizationRendererData*>(rendererData)->shadowType)
	{
		case ShadowType::CSM:
		{
			PrepareRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);
			PrepareRenderFeature("CSM_Visualization_RenderFeature", rendererData);
			break;
		}
		case ShadowType::CASCADE_EVSM:
		{
			PrepareRenderFeature("CascadeEVSM_ShadowCaster_RenderFeature", rendererData);
			PrepareRenderFeature("CascadeEVSM_Visualization_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("ClearColorAttachment_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Geometry_RenderFeature", rendererData, camera, rendererComponents);
	switch (static_cast<ScreenSpaceShadowVisualizationRendererData*>(rendererData)->shadowType)
	{
		case ShadowType::CSM:
		{
			ExcuteRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("CSM_Visualization_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case ShadowType::CASCADE_EVSM:
		{
			ExcuteRenderFeature("CascadeEVSM_ShadowCaster_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("CascadeEVSM_Visualization_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	SubmitRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<ScreenSpaceShadowVisualizationRendererData*>(rendererData)->shadowType)
	{
		case ShadowType::CSM:
		{
			SubmitRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);
			SubmitRenderFeature("CSM_Visualization_RenderFeature", rendererData);
			break;
		}
		case ShadowType::CASCADE_EVSM:
		{
			SubmitRenderFeature("CascadeEVSM_ShadowCaster_RenderFeature", rendererData);
			SubmitRenderFeature("CascadeEVSM_Visualization_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	FinishRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<ScreenSpaceShadowVisualizationRendererData*>(rendererData)->shadowType)
	{
		case ShadowType::CSM:
		{
			FinishRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);
			FinishRenderFeature("CSM_Visualization_RenderFeature", rendererData);
			break;
		}
		case ShadowType::CASCADE_EVSM:
		{
			FinishRenderFeature("CascadeEVSM_ShadowCaster_RenderFeature", rendererData);
			FinishRenderFeature("CascadeEVSM_Visualization_RenderFeature", rendererData);
			break;
		}
	}
}
