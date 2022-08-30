#include "Core/Graphic/Rendering/RendererBase.h"
#include "Utils/Log.h"
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/CoreObject/Thread.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/Command/CommandBuffer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RendererBase>("AirEngine::Core::Graphic::Rendering::RendererBase");
	rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RendererDataBase>("AirEngine::Core::Graphic::Rendering::RendererDataBase");
}

AirEngine::Core::Graphic::Rendering::RendererDataBase::RendererDataBase()
	: _renderFeatureWrappers()
{
}

AirEngine::Core::Graphic::Rendering::RendererDataBase::~RendererDataBase()
{
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Core::Graphic::Rendering::RendererDataBase::RenderFeatureData(std::string renderFeatureName)
{
	return _renderFeatureWrappers[renderFeatureName].renderFeatureData;
}

AirEngine::Core::Graphic::Rendering::RendererBase::RendererBase()
	: _renderFeatures()
{
}

AirEngine::Core::Graphic::Rendering::RendererBase::~RendererBase()
{
}

void AirEngine::Core::Graphic::Rendering::RendererBase::UseRenderFeature(std::string renderFeatureName, RenderFeatureBase* renderFeature)
{
	Utils::Log::Exception("Already contains render feature named: " + renderFeatureName + ".", _renderFeatures.count(renderFeatureName) != 0);
	_renderFeatures[renderFeatureName] = renderFeature;
}

void AirEngine::Core::Graphic::Rendering::RendererBase::PrepareRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData)
{
	_renderFeatures[renderFeatureName]->OnPrepare(rendererData->RenderFeatureData(renderFeatureName));
}

void AirEngine::Core::Graphic::Rendering::RendererBase::ExcuteRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<Renderer::Renderer*> const* rendererComponents)
{
	RendererDataBase::RenderFeatureWrapper* wrapper = &rendererData->_renderFeatureWrappers[renderFeatureName];
	auto renderFeature = _renderFeatures[renderFeatureName];

	wrapper->excuteTask = Core::Graphic::CoreObject::Thread::AddTask(
		[camera, rendererComponents, renderFeature, wrapper](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool) -> void 
		{
			wrapper->commandBuffer = graphicCommandPool->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			renderFeature->OnExcute(wrapper->renderFeatureData, wrapper->commandBuffer, camera, rendererComponents);
		}
	);
	wrapper->excuteTask.get();
}

void AirEngine::Core::Graphic::Rendering::RendererBase::SubmitRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData)
{
	RendererDataBase::RenderFeatureWrapper* wrapper = &rendererData->_renderFeatureWrappers[renderFeatureName];
	//wrapper->excuteTask.get();
	_renderFeatures[renderFeatureName]->OnSubmit(wrapper->renderFeatureData, wrapper->commandBuffer);
}

void AirEngine::Core::Graphic::Rendering::RendererBase::FinishRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData)
{
	RendererDataBase::RenderFeatureWrapper* wrapper = &rendererData->_renderFeatureWrappers[renderFeatureName];
	_renderFeatures[renderFeatureName]->OnFinish(wrapper->renderFeatureData, wrapper->commandBuffer);
	wrapper->commandBuffer->ParentCommandPool()->DestoryCommandBuffer(wrapper->commandBuffer);
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Core::Graphic::Rendering::RendererBase::CreateRendererData(Camera::CameraBase* camera)
{
	AirEngine::Core::Graphic::Rendering::RendererDataBase* rendererData = OnCreateRendererData(camera);
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		rendererData->_renderFeatureWrappers[renderFeaturePair.first].renderFeatureData = renderFeaturePair.second->OnCreateRenderFeatureData(camera);
	}

	OnResolveRendererData(rendererData, camera);
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		renderFeaturePair.second->OnResolveRenderFeatureData(rendererData->_renderFeatureWrappers[renderFeaturePair.first].renderFeatureData, camera);
	}
	return rendererData;
}

void AirEngine::Core::Graphic::Rendering::RendererBase::DestroyRendererData(RendererDataBase* rendererData)
{
	for (const auto& renderFeaturePair : _renderFeatures)
	{
		renderFeaturePair.second->OnDestroyRenderFeatureData(rendererData->_renderFeatureWrappers[renderFeaturePair.first].renderFeatureData);
	}
	OnDestroyRendererData(rendererData);
}

AirEngine::Core::Graphic::Rendering::RenderFeatureBase* AirEngine::Core::Graphic::Rendering::RendererBase::RenderFeature(std::string renderFeatureName)
{
	return _renderFeatures[renderFeatureName];
}
