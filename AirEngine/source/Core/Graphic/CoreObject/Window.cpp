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
#include <QKeyEvent>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <QMouseEvent>
#include <QWheelEvent>

AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow* AirEngine::Core::Graphic::CoreObject::Window::_window = nullptr;
AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer* AirEngine::Core::Graphic::CoreObject::Window::_windowRenderer = nullptr;
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Window::_qVulkanInstance = nullptr;

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    if (!keyEvent->isAutoRepeat()) 
    {
        Logic::CoreObject::Instance::InputManager().InputKey(static_cast<Logic::Manager::InputEventType>(keyEvent->type()), static_cast<Logic::Manager::InputKeyType>(keyEvent->key()));
    }
    keyEvent->accept();
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::keyReleaseEvent(QKeyEvent* keyEvent)
{
    if (!keyEvent->isAutoRepeat())
    {
        Logic::CoreObject::Instance::InputManager().InputKey(static_cast<Logic::Manager::InputEventType>(keyEvent->type()), static_cast<Logic::Manager::InputKeyType>(keyEvent->key()));
    }
    keyEvent->accept();
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::mousePressEvent(QMouseEvent* mouseEvent)
{
    Logic::CoreObject::Instance::InputManager().InputMouse(static_cast<Logic::Manager::InputEventType>(mouseEvent->type()), mouseEvent->button());
    mouseEvent->accept();
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    Logic::CoreObject::Instance::InputManager().InputMouse(static_cast<Logic::Manager::InputEventType>(mouseEvent->type()), mouseEvent->button());
    mouseEvent->accept();
}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindow::wheelEvent(QWheelEvent* wheelEvent)
{
    Logic::CoreObject::Instance::InputManager().InputWheel(static_cast<float>(wheelEvent->angleDelta().y()) * 0.125);
    wheelEvent->accept();
}

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

}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::releaseSwapChainResources()
{

}

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::releaseResources()
{

}

int i = 0;
double duration = 0;

void AirEngine::Core::Graphic::CoreObject::Window::VulkanWindowRenderer::startNextFrame()
{
    //Utils::Log::Message("------------------------------------------------------------------");
    Instance::StartPresentCondition().Awake();
    Instance::EndPresentCondition().Wait();

    if (i == 10)
    {
        auto&& fps = 10.0 / duration;
        Window::VulkanWindow_()->setTitle("AirEngine FPS:" + QString::number(fps, 'f', 2));
        i = 0;
        duration = 0;
    }
    i++;
    duration += Instance::RenderDuration();

    _window->frameReady();
    _window->requestUpdate();
    //Utils::Log::Message("------------------------------------------------------------------");
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
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    _qVulkanInstance = new QVulkanInstance();
    auto supportVersion = _qVulkanInstance->supportedApiVersion();
    _qVulkanInstance->setApiVersion(supportVersion);
    _qVulkanInstance->setLayers(QByteArrayList()
        << "VK_LAYER_KHRONOS_validation"
    );
    if (!_qVulkanInstance->create())
        qFatal("Failed to create Vulkan instance: %d", _qVulkanInstance->errorCode());

    _window = new VulkanWindow();
    _window->setPreferredColorFormats({VkFormat::VK_FORMAT_B8G8R8A8_SRGB});
    _window->setVulkanInstance(_qVulkanInstance);
    uint32_t prefferedPhysicalDeviceIndex = -1;
    {
        auto&& availablePhysicalDevices = _window->availablePhysicalDevices();
        uint32_t i = 0;
        for (const auto& availablePhysicalDevice : availablePhysicalDevices)
        {
            if (availablePhysicalDevice.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                prefferedPhysicalDeviceIndex = i;
                break;
            }
            ++i;
        }
    }
    _window->setPhysicalDeviceIndex(prefferedPhysicalDeviceIndex);
    //auto sde = _window->supportedDeviceExtensions();
    ////_window->setEnabledFeaturesModifier([](VkPhysicalDeviceFeatures& feature)->void {
    ////    
    ////});
    //_window->setDeviceExtensions(QByteArrayList()
    //    << "VK_EXT_shader_atomic_float"
    //    << VK_KHR_MAINTENANCE1_EXTENSION_NAME
    //); 
    //auto queuePrioritieMapPtr = new std::map<uint32_t, std::vector<float>>();
    _window->setQueueCreateInfoModifier([](const VkQueueFamilyProperties* properties, uint32_t queueFamilyCount, QList<VkDeviceQueueCreateInfo>& infos)->void
    {
        auto&& pQueuePriorities = new float[4]{1.0, 1.0, 1.0, 1.0 };
        infos[0].queueCount = 4;
        infos[0].pQueuePriorities = pQueuePriorities;
    });
    
    _window->resize(800, 450);
    _window->show();
}
void AirEngine::Core::Graphic::CoreObject::Window::End()
{

}
