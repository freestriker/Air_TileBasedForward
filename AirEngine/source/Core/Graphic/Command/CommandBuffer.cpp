#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Command/Semaphore.h"

AirEngine::Core::Graphic::Command::CommandBuffer::CommandBuffer(std::string name, CommandPool* parentCommandPool, VkCommandBufferLevel level)
    : _name(name)
    , _parentCommandPool(parentCommandPool)
    , _commandData()
    , _vkCommandBuffer(VK_NULL_HANDLE)
    , _vkFence(VK_NULL_HANDLE)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Utils::Log::Exception("Failed to create synchronization objects for a command buffer.", vkCreateFence(Graphic::CoreObject::Instance::VulkanDevice_(), &fenceInfo, nullptr, &_vkFence));

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = parentCommandPool->_vkCommandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = 1;

    Utils::Log::Exception("Failed to allocate command buffers.", vkAllocateCommandBuffers(Graphic::CoreObject::Instance::VulkanDevice_(), &allocInfo, &_vkCommandBuffer));

}
AirEngine::Core::Graphic::Command::CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(Graphic::CoreObject::Instance::VulkanDevice_(), _parentCommandPool->_vkCommandPool, 1, &_vkCommandBuffer);
    vkDestroyFence(Graphic::CoreObject::Instance::VulkanDevice_(), _vkFence, nullptr);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Reset()
{
    vkResetFences(Graphic::CoreObject::Instance::VulkanDevice_(), 1, &_vkFence);
    vkResetCommandBuffer(_vkCommandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::BeginRecord(VkCommandBufferUsageFlags flag)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flag;

    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::CopyBuffer(Instance::Buffer* srcBuffer, Instance::Buffer* dstBuffer)
{
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcBuffer->Offset();
    copyRegion.dstOffset = dstBuffer->Offset();
    copyRegion.size = srcBuffer->Size();
    vkCmdCopyBuffer(_vkCommandBuffer, srcBuffer->VkBuffer_(), dstBuffer->VkBuffer_(), 1, &copyRegion);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::CopyBuffer(Instance::Buffer* srcBuffer, VkDeviceSize srcOffset, Instance::Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size)
{
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    vkCmdCopyBuffer(_vkCommandBuffer, srcBuffer->VkBuffer_(), dstBuffer->VkBuffer_(), 1, &copyRegion);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::EndRecord()
{
    vkEndCommandBuffer(_vkCommandBuffer);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages, std::vector<Command::Semaphore*> signalSemaphores)
{
    std::vector <VkSemaphore> wait = std::vector <VkSemaphore>(waitSemaphores.size());
    for (uint32_t i = 0; i < waitSemaphores.size(); i++)
    {
        wait[i] = waitSemaphores[i]->VkSemphore_();
    }
    std::vector <VkSemaphore> signal = std::vector <VkSemaphore>(signalSemaphores.size());
    for (uint32_t i = 0; i < signalSemaphores.size(); i++)
    {
        signal[i] = signalSemaphores[i]->VkSemphore_();
    }
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCommandBuffer;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(wait.size());
    submitInfo.pWaitSemaphores = wait.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signal.size());
    submitInfo.pSignalSemaphores = signal.data();

    {
        std::unique_lock<std::mutex> lock(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->mutex);

        vkQueueSubmit(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->queue, 1, &submitInfo, _vkFence);
    }
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Submit(std::vector<Command::Semaphore*> signalSemaphores)
{
    std::vector <VkSemaphore> signal = std::vector <VkSemaphore>(signalSemaphores.size());
    for (uint32_t i = 0; i < signalSemaphores.size(); i++)
    {
        signal[i] = signalSemaphores[i]->VkSemphore_();
    }
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCommandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signal.size());
    submitInfo.pSignalSemaphores = signal.data();

    {
        std::unique_lock<std::mutex> lock(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->mutex);

        vkQueueSubmit(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->queue, 1, &submitInfo, _vkFence);
    }
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Submit()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCommandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    {
        std::unique_lock<std::mutex> lock(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->mutex);

        vkQueueSubmit(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->queue, 1, &submitInfo, _vkFence);
    }
}

void AirEngine::Core::Graphic::Command::CommandBuffer::WaitForFinish()
{
    vkWaitForFences(Graphic::CoreObject::Instance::VulkanDevice_(), 1, &_vkFence, VK_TRUE, UINT64_MAX);
}
