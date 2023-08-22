#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/RenderFeature/Geometry_RenderFeature.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"
#include "Rendering/RenderFeature/TBForward_LightList_RenderFeature.h"
#include "Rendering/RenderFeature/TBForward_Opaque_RenderFeature.h"
#include "Rendering/RenderFeature/TBForward_OIT_DepthPeeling_RenderFeature.h"
#include "Rendering/RenderFeature/SSAO_Occlusion_RenderFeature.h"
#include "Rendering/RenderFeature/AO_Blur_RenderFeature.h"
#include "Rendering/RenderFeature/AO_Cover_RenderFeature.h"
#include "Rendering/RenderFeature/TBForward_OIT_AlphaBuffer_RenderFeature.h"
#include "Rendering/RenderFeature/HBAO_Occlusion_RenderFeature.h"
#include "Rendering/RenderFeature/GTAO_Occlusion_RenderFeature.h"
#include "Rendering/RenderFeature/CSM_ShadowCaster_RenderFeature.h"
#include "Rendering/RenderFeature/TBForward_Transparent_RenderFeature.h"
//#include "Rendering/RenderFeature/IWave_RenderFeature.h"

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
	_description = "This is a renderer for Forward+ that includes: opaque objects, translucent objects, ssao (Depth Stripping, Alpha Buffer, Depth Sorting), cascading shadows.";

	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());

	UseRenderFeature("Geometry_RenderFeature", new RenderFeature::Geometry_RenderFeature());
	
	UseRenderFeature("TBForward_LightList_RenderFeature", new RenderFeature::TBForward_LightList_RenderFeature());
	UseRenderFeature("SSAO_Occlusion_RenderFeature", new RenderFeature::SSAO_Occlusion_RenderFeature());
	UseRenderFeature("SSAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());

	UseRenderFeature("CSM_ShadowCaster_RenderFeature", new RenderFeature::CSM_ShadowCaster_RenderFeature());

	UseRenderFeature("TBForward_Opaque_RenderFeature", new RenderFeature::TBForward_Opaque_RenderFeature());
	UseRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", new RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature());
	UseRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", new RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature());
	UseRenderFeature("TBForward_Transparent_RenderFeature", new RenderFeature::TBForward_Transparent_RenderFeature());

	//UseRenderFeature("IWave_RenderFeature", new RenderFeature::IWave_RenderFeature());
}

AirEngine::Rendering::Renderer::TBForwardRenderer::~TBForwardRenderer()
{
	delete static_cast<RenderFeature::Geometry_RenderFeature*>(RenderFeature("GeometryRenderFeature"));
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
	rendererData->RenderFeatureData<RenderFeature::Background_RenderFeature::Background_RenderFeatureData>("Background_RenderFeature")->needClearColorAttachment = true;
	auto geometryFeatureData = rendererData->RenderFeatureData<RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData>("Geometry_RenderFeature");
	auto lightListFeatureData = rendererData->RenderFeatureData<RenderFeature::TBForward_LightList_RenderFeature::TBForward_LightList_RenderFeatureData>("TBForward_LightList_RenderFeature");
	auto opaqueFeatureData = rendererData->RenderFeatureData<RenderFeature::TBForward_Opaque_RenderFeature::TBForward_Opaque_RenderFeatureData>("TBForward_Opaque_RenderFeature");
	auto depthPeelingFeatureData = rendererData->RenderFeatureData<RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature::TBForward_OIT_DepthPeeling_RenderFeatureData>("TBForward_OIT_DepthPeeling_RenderFeature");
	auto alphaBufferFeatureData = rendererData->RenderFeatureData<RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeatureData>("TBForward_OIT_AlphaBuffer_RenderFeature");
	auto transparentFeatureData = rendererData->RenderFeatureData<RenderFeature::TBForward_Transparent_RenderFeature::TBForward_Transparent_RenderFeatureData>("TBForward_Transparent_RenderFeature");

	auto ssaoFeatureData = rendererData->RenderFeatureData<RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData>("SSAO_Occlusion_RenderFeature");
	ssaoFeatureData->depthTexture = geometryFeatureData->depthTexture;
	ssaoFeatureData->normalTexture = geometryFeatureData->normalTexture;
	ssaoFeatureData->samplePointRadius = 1;

	auto aoBlurFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData>("SSAO_Blur_RenderFeature");
	aoBlurFeatureData->normalTexture = geometryFeatureData->normalTexture;
	aoBlurFeatureData->occlusionTexture = ssaoFeatureData->occlusionTexture;
	aoBlurFeatureData->iterateCount = 2;

	lightListFeatureData->depthTexture = geometryFeatureData->depthTexture;

	auto csmShadowMapFeatureData = rendererData->RenderFeatureData<RenderFeature::CSM_ShadowCaster_RenderFeature::CSM_ShadowCaster_RenderFeatureData>("CSM_ShadowCaster_RenderFeature");
	csmShadowMapFeatureData->frustumSegmentScales = { 0.1, 0.2, 0.3, 0.4 };
	csmShadowMapFeatureData->lightCameraCompensationDistances = { 5, 5, 5, 5 };
	csmShadowMapFeatureData->shadowImageResolutions = 2048;
	csmShadowMapFeatureData->sampleHalfWidth = 2;
	csmShadowMapFeatureData->bias = {
		glm::vec2{ 0.0005f, 0.0075f },
		glm::vec2{ 0.0005f, 0.0095f },
		glm::vec2{ 0.0005f, 0.0105f },
		glm::vec2{ 0.0005f, 0.0115f },
	};

	opaqueFeatureData->opaqueLightIndexListsBuffer = lightListFeatureData->opaqueLightIndexListsBuffer;
	opaqueFeatureData->csmShadowMapRenderFeatureData = csmShadowMapFeatureData;
	opaqueFeatureData->occlusionTexture = aoBlurFeatureData->occlusionTexture;

	depthPeelingFeatureData->transparentLightIndexListsBuffer = lightListFeatureData->transparentLightIndexListsBuffer;
	depthPeelingFeatureData->depthTexture = geometryFeatureData->depthTexture;

	alphaBufferFeatureData->transparentLightIndexListsBuffer = lightListFeatureData->transparentLightIndexListsBuffer;

	transparentFeatureData->transparentLightIndexListsBuffer = lightListFeatureData->transparentLightIndexListsBuffer;
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<TBForwardRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);

	PrepareRenderFeature("Geometry_RenderFeature", rendererData);

	PrepareRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	PrepareRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	PrepareRenderFeature("SSAO_Blur_RenderFeature", rendererData);

	PrepareRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);
	
	PrepareRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->transparentType)
	{
	case TransparentType::DEPTH_PEELING:
		PrepareRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
		break;
	case TransparentType::ALPHA_BUFFER:
		PrepareRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
		break;
	case TransparentType::DEPTH_SORT:
		PrepareRenderFeature("TBForward_Transparent_RenderFeature", rendererData);
		break;
	}

	//PrepareRenderFeature("IWave_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("Geometry_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("TBForward_LightList_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("SSAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("SSAO_Blur_RenderFeature", rendererData, camera, rendererComponents);

	ExcuteRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData, camera, rendererComponents);
	
	ExcuteRenderFeature("TBForward_Opaque_RenderFeature", rendererData, camera, rendererComponents);
	switch (static_cast<TBForwardRendererData*>(rendererData)->transparentType)
	{
	case TransparentType::DEPTH_PEELING:
		ExcuteRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData, camera, rendererComponents);
		break;
	case TransparentType::ALPHA_BUFFER:
		ExcuteRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData, camera, rendererComponents);
		break;
	case TransparentType::DEPTH_SORT:
		ExcuteRenderFeature("TBForward_Transparent_RenderFeature", rendererData, camera, rendererComponents);
		break;
	}

	//ExcuteRenderFeature("IWave_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);

	SubmitRenderFeature("Geometry_RenderFeature", rendererData);

	SubmitRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	SubmitRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	SubmitRenderFeature("SSAO_Blur_RenderFeature", rendererData);

	SubmitRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);

	SubmitRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->transparentType)
	{
	case TransparentType::DEPTH_PEELING:
		SubmitRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
		break;
	case TransparentType::ALPHA_BUFFER:
		SubmitRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
		break;
	case TransparentType::DEPTH_SORT:
		SubmitRenderFeature("TBForward_Transparent_RenderFeature", rendererData);
		break;
	}

	//SubmitRenderFeature("IWave_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);

	FinishRenderFeature("Geometry_RenderFeature", rendererData);

	FinishRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	FinishRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	FinishRenderFeature("SSAO_Blur_RenderFeature", rendererData);

	FinishRenderFeature("CSM_ShadowCaster_RenderFeature", rendererData);
	
	FinishRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->transparentType)
	{
	case TransparentType::DEPTH_PEELING:
		FinishRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
		break;
	case TransparentType::ALPHA_BUFFER:
		FinishRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
		break;
	case TransparentType::DEPTH_SORT:
		FinishRenderFeature("TBForward_Transparent_RenderFeature", rendererData);
		break;
	}

	//FinishRenderFeature("IWave_RenderFeature", rendererData);
}
