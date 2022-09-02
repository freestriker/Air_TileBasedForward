#include "Core/Graphic/Manager/RenderPipelineManager.h"
#include "Utils/Log.h"
#include "Core/Graphic/Rendering/RenderPipelineBase.h"
#include "Core/Graphic/Rendering/RendererBase.h"
#include "Camera/CameraBase.h"

AirEngine::Core::Graphic::Manager::RenderPipelineManager::RenderPipelineManager()
	: _renderPipeline(nullptr)
	, _rendererDatas()
	, _managerMutex()
{
}

AirEngine::Core::Graphic::Manager::RenderPipelineManager::~RenderPipelineManager()
{
}

AirEngine::Core::Graphic::Rendering::RenderPipelineBase* AirEngine::Core::Graphic::Manager::RenderPipelineManager::SwitchRenderPipeline(Rendering::RenderPipelineBase* renderPipeline)
{
	std::lock_guard<std::mutex> locker(_managerMutex);
	
	for (auto& rendererDataPair : _rendererDatas)
	{
		_renderPipeline->Renderer(rendererDataPair.first->RendererName())->DestroyRendererData(rendererDataPair.second);
	}
	
	auto oldRenderPipeline = _renderPipeline;
	_renderPipeline = renderPipeline;

	for (auto& rendererDataPair : _rendererDatas)
	{
		rendererDataPair.second = _renderPipeline->Renderer(rendererDataPair.first->RendererName())->CreateRendererData(rendererDataPair.first);
	}

	return oldRenderPipeline;
}

void AirEngine::Core::Graphic::Manager::RenderPipelineManager::CreateRendererData(Camera::CameraBase* camera)
{
	if (camera == nullptr) return;

	std::lock_guard<std::mutex> locker(_managerMutex);

	Utils::Log::Exception("Camera already has a renderer data.", _rendererDatas.find(camera) != std::end(_rendererDatas));

	auto rendererData = _renderPipeline->Renderer(camera->RendererName())->CreateRendererData(camera);

	_rendererDatas[camera] = rendererData;
}

void AirEngine::Core::Graphic::Manager::RenderPipelineManager::RefreshRendererData(Camera::CameraBase* camera)
{
	if (camera == nullptr) return;

	std::lock_guard<std::mutex> locker(_managerMutex);

	Utils::Log::Exception("Camera does not have a renderer data.", _rendererDatas.find(camera) == std::end(_rendererDatas));

	_renderPipeline->Renderer(camera->RendererName())->DestroyRendererData(_rendererDatas[camera]);
	_rendererDatas[camera] = _renderPipeline->Renderer(camera->RendererName())->CreateRendererData(camera);
}

void AirEngine::Core::Graphic::Manager::RenderPipelineManager::DestroyRendererData(Camera::CameraBase* camera)
{
	if (camera == nullptr) return;

	std::lock_guard<std::mutex> locker(_managerMutex);

	Utils::Log::Exception("Camera does not have a renderer data.", _rendererDatas.find(camera) == std::end(_rendererDatas));

	_renderPipeline->Renderer(camera->RendererName())->DestroyRendererData(_rendererDatas[camera]);

	_rendererDatas.erase(camera);
}

AirEngine::Core::Graphic::Rendering::RenderPipelineBase* AirEngine::Core::Graphic::Manager::RenderPipelineManager::RenderPipeline()
{
	std::lock_guard<std::mutex> locker(_managerMutex);

	return _renderPipeline;
}

AirEngine::Core::Graphic::Rendering::RendererBase* AirEngine::Core::Graphic::Manager::RenderPipelineManager::Renderer(std::string rendererName)
{
	std::lock_guard<std::mutex> locker(_managerMutex);

	return _renderPipeline->Renderer(rendererName);
}

AirEngine::Core::Graphic::Rendering::RendererDataBase* AirEngine::Core::Graphic::Manager::RenderPipelineManager::RendererData(Camera::CameraBase* camera)
{
	if (camera == nullptr) return nullptr;

	std::lock_guard<std::mutex> locker(_managerMutex);

	Utils::Log::Exception("Camera does not have a renderer data.", _rendererDatas.find(camera) == std::end(_rendererDatas));
	
	return _rendererDatas[camera];
}
