#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/MemoryManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Manager/DescriptorSetManager.h"
#include "Utils/Condition.h"
#include "Core/Graphic/Manager/LightManager.h"

std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*> AirEngine::Core::Graphic::CoreObject::Instance::_queues = std::map<std::string, AirEngine::Core::Graphic::CoreObject::Queue*>();
QVulkanInstance* AirEngine::Core::Graphic::CoreObject::Instance::_qVulkanInstance = nullptr;
VkInstance AirEngine::Core::Graphic::CoreObject::Instance::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Core::Graphic::CoreObject::Instance::_vkPhysicalDevice = VK_NULL_HANDLE;
QVulkanDeviceFunctions* AirEngine::Core::Graphic::CoreObject::Instance::_qDeviceFunctions = nullptr;
VkDevice AirEngine::Core::Graphic::CoreObject::Instance::_vkDevice = VK_NULL_HANDLE;

AirEngine::Core::Graphic::Manager::MemoryManager* AirEngine::Core::Graphic::CoreObject::Instance::_memoryManager = nullptr;
AirEngine::Core::Graphic::Manager::RenderPassManager* AirEngine::Core::Graphic::CoreObject::Instance::_renderPassManager = nullptr;
AirEngine::Core::Graphic::Manager::NewRenderPassManager* AirEngine::Core::Graphic::CoreObject::Instance::_newRenderPassManager = nullptr;
AirEngine::Core::Graphic::Manager::DescriptorSetManager* AirEngine::Core::Graphic::CoreObject::Instance::_descriptorSetManager = nullptr;
AirEngine::Core::Graphic::Manager::LightManager* AirEngine::Core::Graphic::CoreObject::Instance::_lightManager = nullptr;

AirEngine::Utils::Condition* AirEngine::Core::Graphic::CoreObject::Instance::_startPresentCondition = nullptr;
AirEngine::Utils::Condition* AirEngine::Core::Graphic::CoreObject::Instance::_endPresentCondition = nullptr;
AirEngine::Utils::Condition* AirEngine::Core::Graphic::CoreObject::Instance::_startRenderCondition = nullptr;
AirEngine::Utils::Condition* AirEngine::Core::Graphic::CoreObject::Instance::_endRenderCondition = nullptr;

std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Graphic::CoreObject::Instance::_lights = std::vector<AirEngine::Core::Logic::Object::Component*>();
std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Graphic::CoreObject::Instance::_cameras = std::vector<AirEngine::Core::Logic::Object::Component*>();
std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Graphic::CoreObject::Instance::_renderers = std::vector<AirEngine::Core::Logic::Object::Component*>();

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

AirEngine::Core::Graphic::Manager::NewRenderPassManager& AirEngine::Core::Graphic::CoreObject::Instance::NewRenderPassManager()
{
	return *_newRenderPassManager;
}

AirEngine::Core::Graphic::Manager::DescriptorSetManager& AirEngine::Core::Graphic::CoreObject::Instance::DescriptorSetManager()
{
	return *_descriptorSetManager;
}

AirEngine::Core::Graphic::Manager::LightManager& AirEngine::Core::Graphic::CoreObject::Instance::LightManager()
{
	return *_lightManager;
}

AirEngine::Utils::Condition& AirEngine::Core::Graphic::CoreObject::Instance::StartPresentCondition()
{
	return *_startPresentCondition;
}

AirEngine::Utils::Condition& AirEngine::Core::Graphic::CoreObject::Instance::EndPresentCondition()
{
	return *_endPresentCondition;
}

AirEngine::Utils::Condition& AirEngine::Core::Graphic::CoreObject::Instance::StartRenderCondition()
{
	return *_startRenderCondition;
}

AirEngine::Utils::Condition& AirEngine::Core::Graphic::CoreObject::Instance::EndRenderCondition()
{
	return *_endRenderCondition;
}

void AirEngine::Core::Graphic::CoreObject::Instance::AddLight(std::vector<Logic::Object::Component*>& lights)
{
	_lights.insert(_lights.end(), lights.begin(), lights.end());
}

void AirEngine::Core::Graphic::CoreObject::Instance::AddCamera(std::vector<Logic::Object::Component*>& cameras)
{
	_cameras.insert(_cameras.end(), cameras.begin(), cameras.end());
}

void AirEngine::Core::Graphic::CoreObject::Instance::AddRenderer(std::vector<Logic::Object::Component*>& renderers)
{
	_renderers.insert(_renderers.end(), renderers.begin(), renderers.end());
}

void AirEngine::Core::Graphic::CoreObject::Instance::ClearLight()
{
	_lights.clear();
}

void AirEngine::Core::Graphic::CoreObject::Instance::ClearCamera()
{
	_cameras.clear();
}

void AirEngine::Core::Graphic::CoreObject::Instance::ClearRenderer()
{
	_renderers.clear();
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
	_qDeviceFunctions = Window::_qVulkanInstance->deviceFunctions(_vkDevice);

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
	_newRenderPassManager = new AirEngine::Core::Graphic::Manager::NewRenderPassManager();
	_descriptorSetManager = new AirEngine::Core::Graphic::Manager::DescriptorSetManager();
	_lightManager = new AirEngine::Core::Graphic::Manager::LightManager();

	_startPresentCondition = new Utils::Condition();
	_endPresentCondition = new Utils::Condition();
	_startRenderCondition = new Utils::Condition();
	_endRenderCondition = new Utils::Condition();
}

AirEngine::Core::Graphic::CoreObject::Queue::Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue)
    : name(name)
    , queueFamilyIndex(queueFamilyIndex)
    , queue(queue)
    , mutex()
{

}
