#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include <map>
#include <string>
#include "Core/Graphic/Instance/Image.h"

AirEngine::Core::Graphic::Manager::RenderPassManager::RenderPassManager()
    : _managerMutex()
    , _renderPasss()
    , _objects()
{
}

AirEngine::Core::Graphic::Manager::RenderPassManager::~RenderPassManager()
{

}

void AirEngine::Core::Graphic::Manager::RenderPassManager::AddRenderPass(RenderPass::RenderPassBase* renderPass)
{
    std::unique_lock<std::mutex> lock(_managerMutex);

    _renderPasss.emplace(renderPass->_name, renderPass);
    renderPass->CreateRenderPass();
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::RemoveRenderPass(std::string name)
{
    std::unique_lock<std::mutex> lock(_managerMutex);

    _renderPasss.erase(name);
}

AirEngine::Core::Graphic::RenderPass::RenderPassBase& AirEngine::Core::Graphic::Manager::RenderPassManager::RenderPass(std::string name)
{
    std::unique_lock<std::mutex> lock(_managerMutex);
    return *_renderPasss[name];
}

AirEngine::Core::Graphic::Manager::RenderPassTarget* AirEngine::Core::Graphic::Manager::RenderPassManager::GetRenderPassObject(std::vector<std::string> renderPassNames, std::map<std::string, Instance::Image*> availableAttachments)
{
    std::unique_lock<std::mutex> lock(_managerMutex);

    std::vector<RenderPass::RenderPassBase*> passes = std::vector<RenderPass::RenderPassBase*>(renderPassNames.size());
    std::vector<Instance::FrameBuffer*> frameBuffers = std::vector<Instance::FrameBuffer*>(renderPassNames.size());
    std::map<std::string, size_t> indexMap = std::map<std::string, size_t>();
    for (size_t i = 0; i < renderPassNames.size(); i++)
    {
        passes[i] = _renderPasss[renderPassNames[i]];
    }
    std::sort(std::begin(passes), std::end(passes),
        [](RenderPass::RenderPassBase*& a, RenderPass::RenderPassBase*& b)
        {
            return a->RenderIndex() < b->RenderIndex();
        }
    );

    VkExtent2D extent{ 0, 0 };
    for (auto& attachment : availableAttachments)
    {
        extent.width = extent.width >= attachment.second->VkExtent3D_().width ? extent.width : attachment.second->VkExtent3D_().width;
        extent.height = extent.height >= attachment.second->VkExtent3D_().height ? extent.height : attachment.second->VkExtent3D_().height;
    }

    for (size_t i = 0; i < passes.size(); i++)
    {
        frameBuffers[i] = new Instance::FrameBuffer(passes[i], availableAttachments, extent);
        indexMap.emplace(renderPassNames[i], i);
    }

    RenderPassTarget* object = new RenderPassTarget();
    object->_passes = passes;
    object->_frameBuffers = frameBuffers;
    object->_indexMap = indexMap;

    _objects.emplace(object);
    return object;
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::DestroyRenderPassObject(RenderPassTarget*& renderPassObject)
{
    if (renderPassObject)
    {
        std::unique_lock<std::mutex> lock(_managerMutex);
        for (const auto& frameBuffer : renderPassObject->_frameBuffers)
        {
            delete frameBuffer;
        }
        _objects.erase(renderPassObject);
        delete renderPassObject;
        renderPassObject = nullptr;
    }
}

AirEngine::Core::Graphic::Instance::FrameBuffer* AirEngine::Core::Graphic::Manager::RenderPassTarget::FrameBuffer(std::string name)
{
    return _frameBuffers[_indexMap[name]];
}

VkExtent2D AirEngine::Core::Graphic::Manager::RenderPassTarget::Extent()
{
    return _extent;
}

AirEngine::Core::Graphic::Manager::RenderPassTarget::RenderPassTarget()
    : _passes()
    , _frameBuffers()
    , _indexMap()
    , _extent()
{
}

AirEngine::Core::Graphic::Manager::RenderPassTarget::~RenderPassTarget()
{
}
