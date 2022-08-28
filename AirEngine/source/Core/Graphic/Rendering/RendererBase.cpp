#include "Core/Graphic/Rendering/RendererBase.h"
#include "Utils/Log.h"
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Camera/CameraBase.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RendererBase>("AirEngine::Core::Graphic::Rendering::RendererBase");
	rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RendererDataBase>("AirEngine::Core::Graphic::Rendering::RendererDataBase");
}

AirEngine::Core::Graphic::Rendering::RendererDataBase::RendererDataBase()
	: _renderFeatureDatas()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase::~RendererDataBase()
{
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Core::Graphic::Rendering::RendererDataBase::RenderFeatureData(std::string renderFeatureName)
{
	return _renderFeatureDatas[renderFeatureName];
}

AirEngine::Core::Graphic::Rendering::RendererBase::RendererBase()
	: _renderFeatures()
{
}

AirEngine::Core::Graphic::Rendering::RendererBase::~RendererBase()
{
}

void AirEngine::Core::Graphic::Rendering::RendererBase::UseRenderFeature(std::string name, RenderFeatureBase* renderFeature)
{
	Utils::Log::Exception("Already contains render feature named: " + name + ".", _renderFeatures.count(name) != 0);
	_renderFeatures[name] = renderFeature;
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Core::Graphic::Rendering::RendererBase::CreateRendererData(Camera::CameraBase* camera)
{
	AirEngine::Core::Graphic::Rendering::RendererDataBase* rendererData = OnCreateRendererData(camera);
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		rendererData->_renderFeatureDatas[renderFeaturePair.first] = renderFeaturePair.second->OnCreateRenderFeatureData(camera);
	}

	OnResolveRendererData(rendererData, camera);
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		renderFeaturePair.second->OnResolveRenderFeatureData(rendererData->_renderFeatureDatas[renderFeaturePair.first], camera);
	}
	return rendererData;
}

void AirEngine::Core::Graphic::Rendering::RendererBase::DestroyRendererData(RendererDataBase* rendererData)
{
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		renderFeaturePair.second->OnDestroyRenderFeatureData(rendererData->_renderFeatureDatas[renderFeaturePair.first]);
	}
	OnDestroyRendererData(rendererData);
}

AirEngine::Core::Graphic::Rendering::RenderFeatureBase* AirEngine::Core::Graphic::Rendering::RendererBase::RenderFeature(std::string name)
{
	return _renderFeatures[name];
}
