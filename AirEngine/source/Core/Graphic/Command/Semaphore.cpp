#include "Core/Graphic/Command/Semaphore.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/VulkanInstance.h"

VkSemaphore AirEngine::Core::Graphic::Command::Semaphore::VkSemphore_()
{
	return _vkSemaphore;
}

AirEngine::Core::Graphic::Command::Semaphore::Semaphore()
	: _vkSemaphore(VK_NULL_HANDLE)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	Utils::Log::Exception("Failed to create semaphore.", vkCreateSemaphore(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), &semaphoreInfo, nullptr, &_vkSemaphore));
}

AirEngine::Core::Graphic::Command::Semaphore::~Semaphore()
{
	vkDestroySemaphore(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), _vkSemaphore, nullptr);
	_vkSemaphore = VK_NULL_HANDLE;
}
