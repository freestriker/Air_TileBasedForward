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
#include "Rendering/RenderFeature/CSM_ShadowMap_RenderFeature.h"

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
	UseRenderFeature("Geometry_RenderFeature", new RenderFeature::Geometry_RenderFeature());
	UseRenderFeature("TBForward_LightList_RenderFeature", new RenderFeature::TBForward_LightList_RenderFeature());
	UseRenderFeature("TBForward_Opaque_RenderFeature", new RenderFeature::TBForward_Opaque_RenderFeature());
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
	UseRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", new RenderFeature::TBForward_OIT_DepthPeeling_RenderFeature());
	UseRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", new RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature());

	UseRenderFeature("SSAO_Occlusion_RenderFeature", new RenderFeature::SSAO_Occlusion_RenderFeature());
	UseRenderFeature("SSAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("SSAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());

	UseRenderFeature("HBAO_Occlusion_RenderFeature", new RenderFeature::HBAO_Occlusion_RenderFeature());
	UseRenderFeature("HBAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("HBAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());

	UseRenderFeature("GTAO_Occlusion_RenderFeature", new RenderFeature::GTAO_Occlusion_RenderFeature());
	UseRenderFeature("GTAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("GTAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());

	UseRenderFeature("CSM_ShadowMap_RenderFeature", new RenderFeature::CSM_ShadowMap_RenderFeature());
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

	///SSAO
	{
		auto ssaoFeatureData = rendererData->RenderFeatureData<RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData>("SSAO_Occlusion_RenderFeature");
		ssaoFeatureData->depthTexture = geometryFeatureData->depthTexture;
		ssaoFeatureData->normalTexture = geometryFeatureData->normalTexture;
		ssaoFeatureData->samplePointRadius = 1;

		auto aoBlurFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData>("SSAO_Blur_RenderFeature");
		aoBlurFeatureData->normalTexture = geometryFeatureData->normalTexture;
		aoBlurFeatureData->occlusionTexture = ssaoFeatureData->occlusionTexture;
		aoBlurFeatureData->iterateCount = 2;

		auto aoCoverFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData>("SSAO_Cover_RenderFeature");
		aoCoverFeatureData->occlusionTexture = ssaoFeatureData->occlusionTexture;
		aoCoverFeatureData->intensity = 1.5f;
	}
	///HBAO
	{
		auto hbaoFeatureData = rendererData->RenderFeatureData<RenderFeature::HBAO_Occlusion_RenderFeature::HBAO_Occlusion_RenderFeatureData>("HBAO_Occlusion_RenderFeature");
		hbaoFeatureData->depthTexture = geometryFeatureData->depthTexture;

		auto aoBlurFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData>("HBAO_Blur_RenderFeature");
		aoBlurFeatureData->normalTexture = geometryFeatureData->normalTexture;
		aoBlurFeatureData->occlusionTexture = hbaoFeatureData->occlusionTexture;
		aoBlurFeatureData->iterateCount = 2;

		auto aoCoverFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData>("HBAO_Cover_RenderFeature");
		aoCoverFeatureData->occlusionTexture = hbaoFeatureData->occlusionTexture;
		aoCoverFeatureData->intensity = 2.0f;
	}
	///GTAO
	{
		auto gtaoFeatureData = rendererData->RenderFeatureData<RenderFeature::GTAO_Occlusion_RenderFeature::GTAO_Occlusion_RenderFeatureData>("GTAO_Occlusion_RenderFeature");
		gtaoFeatureData->depthTexture = geometryFeatureData->depthTexture;

		auto aoBlurFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData>("GTAO_Blur_RenderFeature");
		aoBlurFeatureData->normalTexture = geometryFeatureData->normalTexture;
		aoBlurFeatureData->occlusionTexture = gtaoFeatureData->occlusionTexture;
		aoBlurFeatureData->iterateCount = 2;

		auto aoCoverFeatureData = rendererData->RenderFeatureData<RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData>("GTAO_Cover_RenderFeature");
		aoCoverFeatureData->occlusionTexture = gtaoFeatureData->occlusionTexture;
		aoCoverFeatureData->intensity = 2.0f;
	}

	lightListFeatureData->depthTexture = geometryFeatureData->depthTexture;

	auto csmShadowMapFeatureData = rendererData->RenderFeatureData<RenderFeature::CSM_ShadowMap_RenderFeature::CSM_ShadowMap_RenderFeatureData>("CSM_ShadowMap_RenderFeature");
	csmShadowMapFeatureData->frustumSegmentScales = { 0.1, 0.2, 0.3, 0.4 };
	csmShadowMapFeatureData->lightCameraCompensationDistances = { 5, 5, 5, 5 };
	csmShadowMapFeatureData->shadowImageResolutions = { 1024, 1024, 1024, 1024 };

	opaqueFeatureData->opaqueLightIndexListsBuffer = lightListFeatureData->opaqueLightIndexListsBuffer;
	opaqueFeatureData->csmShadowMapRenderFeatureData = csmShadowMapFeatureData;

	depthPeelingFeatureData->transparentLightIndexListsBuffer = lightListFeatureData->transparentLightIndexListsBuffer;
	depthPeelingFeatureData->depthTexture = geometryFeatureData->depthTexture;

	alphaBufferFeatureData->transparentLightIndexListsBuffer = lightListFeatureData->transparentLightIndexListsBuffer;


}

void AirEngine::Rendering::Renderer::TBForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<TBForwardRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	PrepareRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			PrepareRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			PrepareRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			PrepareRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			break;
		}
	}
	PrepareRenderFeature("CSM_ShadowMap_RenderFeature", rendererData);
	
	PrepareRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	PrepareRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			PrepareRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			PrepareRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			PrepareRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
	switch (static_cast<TBForwardRendererData*>(rendererData)->oitType)
	{
		case OitType::DEPTH_PEELING:
		{
			PrepareRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
			break;
		}
		case OitType::ALPHA_BUFFER:
		{
			PrepareRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Geometry_RenderFeature", rendererData, camera, rendererComponents);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			ExcuteRenderFeature("SSAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("SSAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::HBAO:
		{
			ExcuteRenderFeature("HBAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("HBAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::GTAO:
		{
			ExcuteRenderFeature("GTAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("GTAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
	}
	ExcuteRenderFeature("CSM_ShadowMap_RenderFeature", rendererData, camera, rendererComponents);
	
	ExcuteRenderFeature("TBForward_LightList_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("TBForward_Opaque_RenderFeature", rendererData, camera, rendererComponents);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			ExcuteRenderFeature("SSAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::HBAO:
		{
			ExcuteRenderFeature("HBAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::GTAO:
		{
			ExcuteRenderFeature("GTAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
	}
	switch (static_cast<TBForwardRendererData*>(rendererData)->oitType)
	{
		case OitType::DEPTH_PEELING:
		{
			ExcuteRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case OitType::ALPHA_BUFFER:
		{
			ExcuteRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	SubmitRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			SubmitRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			SubmitRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			SubmitRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			break;
		}
	}
	SubmitRenderFeature("CSM_ShadowMap_RenderFeature", rendererData);

	SubmitRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	SubmitRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			SubmitRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			SubmitRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			SubmitRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
	switch (static_cast<TBForwardRendererData*>(rendererData)->oitType)
	{
		case OitType::DEPTH_PEELING:
		{
			SubmitRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
			break;
		}
		case OitType::ALPHA_BUFFER:
		{
			SubmitRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::TBForwardRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	FinishRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			FinishRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			FinishRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			FinishRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			break;
		}
	}
	FinishRenderFeature("CSM_ShadowMap_RenderFeature", rendererData);
	
	FinishRenderFeature("TBForward_LightList_RenderFeature", rendererData);
	FinishRenderFeature("TBForward_Opaque_RenderFeature", rendererData);
	switch (static_cast<TBForwardRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			FinishRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			FinishRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			FinishRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
	switch (static_cast<TBForwardRendererData*>(rendererData)->oitType)
	{
		case OitType::DEPTH_PEELING:
		{
			FinishRenderFeature("TBForward_OIT_DepthPeeling_RenderFeature", rendererData);
			break;
		}
		case OitType::ALPHA_BUFFER:
		{
			FinishRenderFeature("TBForward_OIT_AlphaBuffer_RenderFeature", rendererData);
			break;
		}
	}
}
