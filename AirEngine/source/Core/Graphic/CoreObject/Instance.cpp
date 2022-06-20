#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/MemoryManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Manager/RenderPassManager.h"

std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*> AirEngine::Core::Graphic::CoreObject::Instance::_queues = std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*>();
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Instance::_qVulkanInstance = nullptr;
VkInstance AirEngine::Core::Graphic::CoreObject::Instance::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::Instance::_vkPhysicalDevice = VK_NULL_HANDLE;
QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::Instance::_qDeviceFunctions = nullptr;
VkDevice AirEngine::Core::Graphic::CoreObject::Instance::_vkDevice = VK_NULL_HANDLE;

AirEngine::Core::Graphic::Manager::MemoryManager* AirEngine::Core::Graphic::CoreObject::Instance::_memoryManager = nullptr;
AirEngine::Core::Graphic::Manager::RenderPassManager* AirEngine::Core::Graphic::CoreObject::Instance::_renderPassManager = nullptr;

AirEngine::Core::Graphic::CoreObject::Queue* AirEngine::Core::Graphic::CoreObject::Instance::Queue_(std::string name)
{
    return _queues[name];
}

QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Instance::QVulkanInstance_()
{
    return _qVulkanInstance;
}

VkInstance AirEngine::Core::Graphic::CoreObject::Instance::VkInstance_()
{
    return _vkInstance;
}

VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::Instance::VkPhysicalDevice_()
{
    return _vkPhysicalDevice;
}

QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::Instance::QVulkanDeviceFunctions_()
{
    return _qDeviceFunctions;
}

VkDevice AirEngine::Core::Graphic::CoreObject::Instance::VkDevice_()
{
    return _vkDevice;
}

AirEngine::Core::Graphic::Manager::MemoryManager& AirEngine::Core::Graphic::CoreObject::Instance::MemoryManager()
{
    return *_memoryManager;
}

AirEngine::Core::Graphic::Manager::RenderPassManager& AirEngine::Core::Graphic::CoreObject::Instance::RenderPassManager()
{
	return *_renderPassManager;
}

AirEngine::Core::Graphic::CoreObject::Instance::Instance()
{

}

void AirEngine::Core::Graphic::CoreObject::Instance::Init()
{
	_qVulkanInstance = Window::_window->vulkanInstance();
	_vkInstance = Window::_window->vulkanInstance()->vkInstance();
	_vkPhysicalDevice = Window::_window->physicalDevice();
	_vkDevice = Window::_window->device();
	_qDeviceFunctions = Window::_vulkanInstance->deviceFunctions(_vkDevice);

	VkQueue queue = VK_NULL_HANDLE;
	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 0, &queue);
	_queues["PresentQueue"] = new Queue("PresentQueue", 0, queue);

	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 1, &queue);
	_queues["GraphicQueue"] = new Queue("GraphicQueue", 0, queue);

	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 2, &queue);
	_queues["TransferQueue"] = new Queue("TransferQueue", 0, queue);

	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 3, &queue);
	_queues["ComputeQueue"] = new Queue("ComputeQueue", 0, queue);

	_memoryManager = new AirEngine::Core::Graphic::Manager::MemoryManager(32 * 1024 * 1024);
	_renderPassManager = new AirEngine::Core::Graphic::Manager::RenderPassManager();
}

AirEngine::Core::Graphic::CoreObject::Queue::Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue)
    : name(name)
    , queueFamilyIndex(queueFamilyIndex)
    , queue(queue)
    , mutex()
{

}
