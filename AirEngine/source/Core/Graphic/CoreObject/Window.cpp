#include "Core/Graphic/CoreObject/Window.h"
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
#include "Core/Logic/CoreObject/Thread.h"
#include "Core/IO/CoreObject/Thread.h"
#include "Core/Graphic/CoreObject/Thread.h"
#include "Utils/Log.h"
#include "Utils/Condition.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Audio/CoreObject/Thread.h"

AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow* AirEngine::Core::Graphic::CoreObject::Window::_window = nullptr;
AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer* AirEngine::Core::Graphic::CoreObject::Window::_windowRenderer = nullptr;
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Window::_qVulkanInstance = nullptr;

QVulkanWindowRenderer* AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::createRenderer()
{
    _windowRenderer = new VulkanWindowRenderer(this);
    return _windowRenderer;
}

AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::VulkanWindowRenderer(QVulkanWindow* window)
{
    window->setSampleCount(1);
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::initResources()
{
    qDebug("AirEngine::Core::Graphic::CoreObject::Thread::VulkanWindowRenderer::initResources()");
    AirEngine::Core::Graphic::CoreObject::Thread::Init();
    AirEngine::Core::Audio::CoreObject::Thread::Init();
    AirEngine::Core::IO::CoreObject::Thread::Init();
    AirEngine::Core::Logic::CoreObject::Thread::Init();

    AirEngine::Core::Graphic::CoreObject::Thread::Start();
    AirEngine::Core::Graphic::CoreObject::Thread::WaitForStartFinish();

    AirEngine::Core::Audio::CoreObject::Thread::Start();
    AirEngine::Core::Audio::CoreObject::Thread::WaitForStartFinish();

    AirEngine::Core::IO::CoreObject::Thread::Start();
    AirEngine::Core::IO::CoreObject::Thread::WaitForStartFinish();

    AirEngine::Core::Logic::CoreObject::Thread::Start();
    AirEngine::Core::Logic::CoreObject::Thread::WaitForStartFinish();
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::initSwapChainResources()
{
    qDebug("AirEngine::Core::Graphic::CoreObject::Thread::VulkanWindowRenderer::initSwapChainResources()");
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::releaseSwapChainResources()
{
    qDebug("AirEngine::Core::Graphic::CoreObject::Thread::VulkanWindowRenderer::releaseSwapChainResources()");
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::releaseResources()
{
    qDebug("AirEngine::Core::Graphic::CoreObject::Thread::VulkanWindowRenderer::releaseResources()");
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::startNextFrame()
{
    Utils::Log::Message("------------------------------------------------------------------");
    Utils::Log::Message("Instance::StartPresentCondition().Awake()");
    Instance::StartPresentCondition().Awake();
    Instance::EndPresentCondition().Wait();
    Utils::Log::Message("Instance::EndPresentCondition().Wait()");

    Utils::Log::Message("FrameReady()");
    _window->frameReady();
    _window->requestUpdate();
    Utils::Log::Message("------------------------------------------------------------------");
}
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Window::QVulkanInstance_()
{
    return _qVulkanInstance;
}
AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow* AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow_()
{
    return _window;
}
AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer* AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer_()
{
    return _windowRenderer;
}
VkExtent2D AirEngine::Core::Graphic::CoreObject::Window::Extent()
{
    return { static_cast<uint32_t>(_window->size().width()), static_cast<uint32_t>(_window->size().height()) };
}
void AirEngine::Core::Graphic::CoreObject::Window::Init()
{

}
void AirEngine::Core::Graphic::CoreObject::Window::Start()
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    _qVulkanInstance = new QVulkanInstance();
    auto supportVersion = _qVulkanInstance->supportedApiVersion();
    _qVulkanInstance->setApiVersion(supportVersion);
    _qVulkanInstance->setLayers(QByteArrayList()
        << "VK_LAYER_KHRONOS_validation"
        << "VK_LAYER_RENDERDOC_Capture"
        << "VK_LAYER_GOOGLE_threading"
        << "VK_LAYER_LUNARG_parameter_validation"
        << "VK_LAYER_LUNARG_object_tracker"
        << "VK_LAYER_LUNARG_core_validation"
        << "VK_LAYER_LUNARG_image"
        << "VK_LAYER_LUNARG_swapchain"
        << "VK_LAYER_GOOGLE_unique_objects"
    );
    if (!_qVulkanInstance->create())
        qFatal("Failed to create Vulkan instance: %d", _qVulkanInstance->errorCode());

    _window = new VulkanWindow();
    _window->setPreferredColorFormats({VkFormat::VK_FORMAT_B8G8R8A8_SRGB});
    _window->setVulkanInstance(_qVulkanInstance);
    _window->setDeviceExtensions(QByteArrayList()
        << "VK_EXT_shader_atomic_float"
    );
    auto queuePrioritieMapPtr = new std::map<uint32_t, std::vector<float>>();
    _window->setQueueCreateInfoModifier([](const VkQueueFamilyProperties* properties, uint32_t queueFamilyCount, QList<VkDeviceQueueCreateInfo>& infos)->void
    {
        auto p = new std::vector<float>({0, 0, 0, 0});
        infos[0].queueCount = 4;
        infos[0].pQueuePriorities = p->data();
    });
    _window->resize(800, 450);
    _window->show();

}
void AirEngine::Core::Graphic::CoreObject::Window::End()
{

}
