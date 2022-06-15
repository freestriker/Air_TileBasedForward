#include "Core/Graphic/CoreObject/VulkanInstance.h"

std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*> AirEngine::Core::Graphic::CoreObject::VulkanInstance::_queues = std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*>();
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::VulkanInstance::_qVulkanInstance = nullptr;
VkInstance AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkPhysicalDevice = VK_NULL_HANDLE;
QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::VulkanInstance::_qDeviceFunctions = nullptr;
VkDevice AirEngine::Core::Graphic::CoreObject::VulkanInstance::_vkDevice = VK_NULL_HANDLE;

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

AirEngine::Core::Graphic::CoreObject::VulkanInstance::VulkanInstance()
{

}

AirEngine::Core::Graphic::CoreObject::Queue::Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue)
    : name(name)
    , queueFamilyIndex(queueFamilyIndex)
    , queue(queue)
    , mutex()
{

}
