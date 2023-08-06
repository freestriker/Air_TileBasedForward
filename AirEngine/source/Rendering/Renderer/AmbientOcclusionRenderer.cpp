#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
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
#include "Rendering/RenderFeature/ClearColorAttachment_RenderFeature.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::AmbientOcclusionRenderer>("AirEngine::Rendering::Renderer::AmbientOcclusionRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData>("AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	_description = "This is a renderer for visualizing ambient occlusion effects, including: ssao, hbao and gtao.";

	UseRenderFeature("Geometry_RenderFeature", new RenderFeature::Geometry_RenderFeature());
	UseRenderFeature("ClearColorAttachment_RenderFeature", new RenderFeature::ClearColorAttachment_RenderFeature());

	UseRenderFeature("SSAO_Occlusion_RenderFeature", new RenderFeature::SSAO_Occlusion_RenderFeature());
	UseRenderFeature("SSAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("SSAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());

	UseRenderFeature("HBAO_Occlusion_RenderFeature", new RenderFeature::HBAO_Occlusion_RenderFeature());
	UseRenderFeature("HBAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("HBAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());

	UseRenderFeature("GTAO_Occlusion_RenderFeature", new RenderFeature::GTAO_Occlusion_RenderFeature());
	UseRenderFeature("GTAO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("GTAO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());
}

AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::~AmbientOcclusionRenderer()
{
	delete static_cast<RenderFeature::Geometry_RenderFeature*>(RenderFeature("GeometryRenderFeature"));
}

AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData::AmbientOcclusionRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData::~AmbientOcclusionRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new AmbientOcclusionRendererData();
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	auto clearFeatureData = rendererData->RenderFeatureData<RenderFeature::ClearColorAttachment_RenderFeature::ClearColorAttachment_RenderFeatureData>("ClearColorAttachment_RenderFeature");
	clearFeatureData->clearValue = { 255, 255, 255, 255 };

	auto geometryFeatureData = rendererData->RenderFeatureData<RenderFeature::Geometry_RenderFeature::Geometry_RenderFeatureData>("Geometry_RenderFeature");

	///SSAO
	{
		auto ssaoFeatureData = rendererData->RenderFeatureData<RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData>("SSAO_Occlusion_RenderFeature");
		ssaoFeatureData->depthTexture = geometryFeatureData->depthTexture;
		ssaoFeatureData->normalTexture = geometryFeatureData->normalTexture;
		ssaoFeatureData->samplePointRadius = 0.2;

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
		aoCoverFeatureData->intensity = 2.5f;
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
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<AmbientOcclusionRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	PrepareRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<AmbientOcclusionRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			PrepareRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			PrepareRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			PrepareRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			PrepareRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			PrepareRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			PrepareRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			PrepareRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("ClearColorAttachment_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Geometry_RenderFeature", rendererData, camera, rendererComponents);
	switch (static_cast<AmbientOcclusionRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			ExcuteRenderFeature("SSAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("SSAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("SSAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::HBAO:
		{
			ExcuteRenderFeature("HBAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("HBAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("HBAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
		case AoType::GTAO:
		{
			ExcuteRenderFeature("GTAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("GTAO_Blur_RenderFeature", rendererData, camera, rendererComponents);
			ExcuteRenderFeature("GTAO_Cover_RenderFeature", rendererData, camera, rendererComponents);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	SubmitRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<AmbientOcclusionRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			SubmitRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			SubmitRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			SubmitRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			SubmitRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			SubmitRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			SubmitRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			SubmitRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
}

void AirEngine::Rendering::Renderer::AmbientOcclusionRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("ClearColorAttachment_RenderFeature", rendererData);
	FinishRenderFeature("Geometry_RenderFeature", rendererData);
	switch (static_cast<AmbientOcclusionRendererData*>(rendererData)->aoType)
	{
		case AoType::SSAO:
		{
			FinishRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("SSAO_Blur_RenderFeature", rendererData);
			FinishRenderFeature("SSAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::HBAO:
		{
			FinishRenderFeature("HBAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("HBAO_Blur_RenderFeature", rendererData);
			FinishRenderFeature("HBAO_Cover_RenderFeature", rendererData);
			break;
		}
		case AoType::GTAO:
		{
			FinishRenderFeature("GTAO_Occlusion_RenderFeature", rendererData);
			FinishRenderFeature("GTAO_Blur_RenderFeature", rendererData);
			FinishRenderFeature("GTAO_Cover_RenderFeature", rendererData);
			break;
		}
	}
}
