#include "Core/Graphic/Command/Fence.h"
#include "Core/Graphic/CoreObject/VulkanInstance.h"
#include "Utils/Log.h"

AirEngine::Core::Graphic::Command::Fence::Fence(VkFenceCreateFlags flag)
	: _vkFence(VK_NULL_HANDLE)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flag;

    Utils::Log::Exception("Failed to create synchronization objects for a frame.", vkCreateFence(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), &fenceInfo, nullptr, &_vkFence));
}

AirEngine::Core::Graphic::Command::Fence::Fence()
    : Fence(VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT)
{
}

AirEngine::Core::Graphic::Command::Fence::~Fence()
{
    vkDestroyFence(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), _vkFence, nullptr);
}

VkFence AirEngine::Core::Graphic::Command::Fence::VkFence_()
{
    return _vkFence;
}

void AirEngine::Core::Graphic::Command::Fence::Reset()
{
    vkResetFences(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), 1, &_vkFence);
}

void AirEngine::Core::Graphic::Command::Fence::Wait()
{
    vkWaitForFences(Graphic::CoreObject::VulkanInstance::VulkanDevice_(), 1, &_vkFence, VK_TRUE, UINT64_MAX);
}
