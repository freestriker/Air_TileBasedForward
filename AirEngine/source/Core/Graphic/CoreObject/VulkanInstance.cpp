#include "Core/Graphic/CoreObject/VulkanInstance.h"
#include "Core/Graphic/Manager/MemoryManager.h"
#include "Core/Graphic/CoreObject/Window.h"

std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*> AirEngine::Core::Graphic::CoreObject::VulkanInstance::_queues = std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*>();
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::VulkanInstance::_qVulkanInstance = nullptr;
VkInstance AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkPhysicalDevice = VK_NULL_HANDLE;
QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::VulkanInstance::_qDeviceFunctions = nullptr;
VkDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkDevice = VK_NULL_HANDLE;

AirEngine::Core::Graphic::Manager::MemoryManager* AirEngine::Core::Graphic::CoreObject::VulkanInstance::_memoryManager = nullptr;

const AirEngine::Core::Graphic::CoreObject::Queue* AirEngine::Core::Graphic::CoreObject::VulkanInstance::GetQueue(std::string name)
{
    return _queues[name];
}

QVulkanInstance* AirEngine::Core::Graphic::CoreObject::VulkanInstance::QVulkanInstance_()
{
    return _qVulkanInstance;
}

VkInstance AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanInstance_()
{
    return _vkInstance;
}

VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanPhysicalDevice_()
{
    return _vkPhysicalDevice;
}

QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanDeviceFunctions_()
{
    return _qDeviceFunctions;
}

VkDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanDevice_()
{
    return _vkDevice;
}

AirEngine::Core::Graphic::Manager::MemoryManager& AirEngine::Core::Graphic::CoreObject::VulkanInstance::MemoryManager()
{
    return *_memoryManager;
}

AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanInstance()
{

}

void AirEngine::Core::Graphic::CoreObject::VulkanInstance::Init()
{
	_qVulkanInstance = Window::_window->vulkanInstance();
	_vkInstance = Window::_window->vulkanInstance()->vkInstance();
	_vkPhysicalDevice = Window::_window->physicalDevice();
	_vkDevice = Window::_window->device();
	_qDeviceFunctions = Window::_vulkanInstance->deviceFunctions(_vkDevice);

	VkQueue queue = VK_NULL_HANDLE;
	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 1, &queue);
	_queues["GraphicQueue"] = new Queue("GraphicQueue", 0, queue);
	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 2, &queue);
	_queues["TransferQueue"] = new Queue("TransferQueue", 0, queue);
	_qDeviceFunctions->vkGetDeviceQueue(_vkDevice, 0, 3, &queue);
	_queues["ComputeQueue"] = new Queue("ComputeQueue", 0, queue);

	_memoryManager = new AirEngine::Core::Graphic::Manager::MemoryManager(32 * 1024 * 1024);
}

AirEngine::Core::Graphic::CoreObject::Queue::Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue)
    : name(name)
    , queueFamilyIndex(queueFamilyIndex)
    , queue(queue)
    , mutex()
{

}
