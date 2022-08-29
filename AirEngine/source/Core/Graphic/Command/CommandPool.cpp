#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"

AirEngine::Core::Graphic::Command::CommandPool::CommandPool(std::string queueName, VkCommandPoolCreateFlags flag)
    : _namedCommandBuffers()
    , _queueName(queueName)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = flag;
    poolInfo.queueFamilyIndex = Graphic::CoreObject::Instance::Queue_(std::string(queueName))->queueFamilyIndex;

    Utils::Log::Exception("Failed to create command pool.", vkCreateCommandPool(Graphic::CoreObject::Instance::VkDevice_(), &poolInfo, nullptr, &_vkCommandPool));
}

AirEngine::Core::Graphic::Command::CommandPool::~CommandPool()
{
    for (auto& commandBuffer : _namedCommandBuffers)
    {
        delete commandBuffer.second;
    }
    vkDestroyCommandPool(Graphic::CoreObject::Instance::VkDevice_(), _vkCommandPool, nullptr);
}

VkCommandPool AirEngine::Core::Graphic::Command::CommandPool::VkCommandPool_()
{
    return _vkCommandPool;
}

AirEngine::Core::Graphic::Command::CommandBuffer* AirEngine::Core::Graphic::Command::CommandPool::CreateCommandBuffer(std::string name, VkCommandBufferLevel level)
{
    auto p = new Graphic::Command::CommandBuffer(name, this, level);
    _namedCommandBuffers.emplace(std::string(name), p);
    return p;
}

AirEngine::Core::Graphic::Command::CommandBuffer* AirEngine::Core::Graphic::Command::CommandPool::CreateCommandBuffer(VkCommandBufferLevel level)
{
    auto p = new Graphic::Command::CommandBuffer("TemporaryCommandBuffer", this, level);
    _commandBuffers.emplace(p);
    return p;
}

AirEngine::Core::Graphic::Command::CommandBuffer* AirEngine::Core::Graphic::Command::CommandPool::GetCommandBuffer(std::string name)
{
    return _namedCommandBuffers[name];
}

void AirEngine::Core::Graphic::Command::CommandPool::DestoryCommandBuffer(CommandBuffer* commandBuffer)
{
    if (_commandBuffers.count(commandBuffer))
    {
        _commandBuffers.erase(commandBuffer);
        delete commandBuffer;
    }
    else
    {
        _namedCommandBuffers.erase(commandBuffer->_name);
        delete commandBuffer;
    }
}

void AirEngine::Core::Graphic::Command::CommandPool::Reset()
{
    for (const auto& commandBuffer : _namedCommandBuffers)
    {
        delete commandBuffer.second;
    }
    _namedCommandBuffers.clear();
    vkResetCommandPool(Graphic::CoreObject::Instance::VkDevice_(), _vkCommandPool, VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}
