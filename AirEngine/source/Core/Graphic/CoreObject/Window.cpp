#include "Core/Graphic/CoreObject/Window.h"

AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow* AirEngine::Core::Graphic::CoreObject::Window::_window = nullptr;
AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer* AirEngine::Core::Graphic::CoreObject::Window::_windowRenderer = nullptr;
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Window::_vulkanInstance = nullptr;

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
    qDebug("AirEngine::Core::Graphic::CoreObject::Thread::VulkanWindowRenderer::startNextFrame()");
    _window->frameReady();
    _window->requestUpdate();
}
void AirEngine::Core::Graphic::CoreObject::Window::Init()
{

}
void AirEngine::Core::Graphic::CoreObject::Window::Start()
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    _vulkanInstance = new QVulkanInstance();

    _vulkanInstance->setLayers(QByteArrayList()
        << "VK_LAYER_GOOGLE_threading"
        << "VK_LAYER_LUNARG_parameter_validation"
        << "VK_LAYER_LUNARG_object_tracker"
        << "VK_LAYER_LUNARG_core_validation"
        << "VK_LAYER_RENDERDOC_Capture"
        << "VK_LAYER_LUNARG_image"
        << "VK_LAYER_LUNARG_swapchain"
        << "VK_LAYER_GOOGLE_unique_objects");

    if (!_vulkanInstance->create())
        qFatal("Failed to create Vulkan instance: %d", _vulkanInstance->errorCode());

    _window = new VulkanWindow();
    _window->setVulkanInstance(_vulkanInstance);

    _window->resize(1024, 768);
    _window->show();

}
void AirEngine::Core::Graphic::CoreObject::Window::End()
{

}
