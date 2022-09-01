#include "Rendering/Renderer/ForwardRenderer.h"
#include "Rendering/RenderFeature/GeometryRenderFeature.h"
#include "Rendering/RenderFeature/Forward_Opaque_RenderFeature.h"
#include "Rendering/RenderFeature/Background_RenderFeature.h"
#include "Rendering/RenderFeature/Forward_Transparent_RenderFeature.h"
#include "Rendering/RenderFeature/SSAO_Occlusion_RenderFeature.h"
#include "Rendering/RenderFeature/AO_Blur_RenderFeature.h"
#include "Rendering/RenderFeature/AO_Cover_RenderFeature.h"
#include <glm/glm.hpp>
#include "Camera/CameraBase.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::Renderer::ForwardRenderer>("AirEngine::Rendering::Renderer::ForwardRenderer")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData>("AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRenderer()
	: Core::Graphic::Rendering::RendererBase()
{
	UseRenderFeature("GeometryRenderFeature", new RenderFeature::GeometryRenderFeature());
	UseRenderFeature("Forward_Opaque_RenderFeature", new RenderFeature::Forward_Opaque_RenderFeature());
	UseRenderFeature("Forward_Transparent_RenderFeature", new RenderFeature::Forward_Transparent_RenderFeature());
	UseRenderFeature("Background_RenderFeature", new RenderFeature::Background_RenderFeature());
	UseRenderFeature("SSAO_Occlusion_RenderFeature", new RenderFeature::SSAO_Occlusion_RenderFeature());
	UseRenderFeature("AO_Blur_RenderFeature", new RenderFeature::AO_Blur_RenderFeature());
	UseRenderFeature("AO_Cover_RenderFeature", new RenderFeature::AO_Cover_RenderFeature());
}

AirEngine::Rendering::Renderer::ForwardRenderer::~ForwardRenderer()
{
	delete static_cast<RenderFeature::GeometryRenderFeature*>(RenderFeature("GeometryRenderFeature"));
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData::ForwardRendererData()
	: Core::Graphic::Rendering::RendererDataBase()
{
}

AirEngine::Rendering::Renderer::ForwardRenderer::ForwardRendererData::~ForwardRendererData()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Rendering::Renderer::ForwardRenderer::OnCreateRendererData(Camera::CameraBase* camera)
{
	return new ForwardRendererData();
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)
{
	static_cast<RenderFeature::Background_RenderFeature::Background_RenderFeatureData*>(rendererData->RenderFeatureData("Background_RenderFeature"))->needClearColorAttachment = true;
	static_cast<RenderFeature::Forward_Opaque_RenderFeature::Forward_Opaque_RenderFeatureData*>(rendererData->RenderFeatureData("Forward_Opaque_RenderFeature"))->needClearColorAttachment = false;
	auto geometryFeatureData = static_cast<RenderFeature::GeometryRenderFeature::GeometryRenderFeatureData*>(rendererData->RenderFeatureData("GeometryRenderFeature"));
	auto ssaoFeatureData = static_cast<RenderFeature::SSAO_Occlusion_RenderFeature::SSAO_Occlusion_RenderFeatureData*>(rendererData->RenderFeatureData("SSAO_Occlusion_RenderFeature"));
	auto aoBlurFeatureData = static_cast<RenderFeature::AO_Blur_RenderFeature::AO_Blur_RenderFeatureData*>(rendererData->RenderFeatureData("AO_Blur_RenderFeature"));
	auto aoCoverFeatureData = static_cast<RenderFeature::AO_Cover_RenderFeature::AO_Cover_RenderFeatureData*>(rendererData->RenderFeatureData("AO_Cover_RenderFeature"));
	
	ssaoFeatureData->depthTexture = geometryFeatureData->depthTexture;
	ssaoFeatureData->normalTexture = geometryFeatureData->normalTexture;

	aoBlurFeatureData->normalTexture = geometryFeatureData->normalTexture;
	aoBlurFeatureData->occlusionTexture = ssaoFeatureData->occlusionTexture;
	aoBlurFeatureData->iterateCount = 2;

	aoCoverFeatureData->occlusionTexture = ssaoFeatureData->occlusionTexture;
	aoCoverFeatureData->intensity = 1.5f;
}

void AirEngine::Rendering::Renderer::ForwardRenderer::OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	delete static_cast<ForwardRendererData*>(rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	PrepareRenderFeature("Background_RenderFeature", rendererData);
	PrepareRenderFeature("GeometryRenderFeature", rendererData);
	PrepareRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	PrepareRenderFeature("AO_Blur_RenderFeature", rendererData);
	PrepareRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	PrepareRenderFeature("AO_Cover_RenderFeature", rendererData);
	PrepareRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	ExcuteRenderFeature("Background_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("GeometryRenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("SSAO_Occlusion_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("AO_Blur_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Forward_Opaque_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("AO_Cover_RenderFeature", rendererData, camera, rendererComponents);
	ExcuteRenderFeature("Forward_Transparent_RenderFeature", rendererData, camera, rendererComponents);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	SubmitRenderFeature("Background_RenderFeature", rendererData);
	SubmitRenderFeature("GeometryRenderFeature", rendererData);
	SubmitRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	SubmitRenderFeature("AO_Blur_RenderFeature", rendererData);
	SubmitRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	SubmitRenderFeature("AO_Cover_RenderFeature", rendererData);
	SubmitRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}

void AirEngine::Rendering::Renderer::ForwardRenderer::FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData)
{
	FinishRenderFeature("Background_RenderFeature", rendererData);
	FinishRenderFeature("GeometryRenderFeature", rendererData);
	FinishRenderFeature("SSAO_Occlusion_RenderFeature", rendererData);
	FinishRenderFeature("AO_Blur_RenderFeature", rendererData);
	FinishRenderFeature("Forward_Opaque_RenderFeature", rendererData);
	FinishRenderFeature("AO_Cover_RenderFeature", rendererData);
	FinishRenderFeature("Forward_Transparent_RenderFeature", rendererData);
}
 