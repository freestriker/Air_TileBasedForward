#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Command/Semaphore.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"

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

    Utils::Log::Exception("Failed to create synchronization objects for a command buffer.", vkCreateFence(Graphic::CoreObject::Instance::VkDevice_(), &fenceInfo, nullptr, &_vkFence));

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = parentCommandPool->_vkCommandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = 1;

    Utils::Log::Exception("Failed to allocate command buffers.", vkAllocateCommandBuffers(Graphic::CoreObject::Instance::VkDevice_(), &allocInfo, &_vkCommandBuffer));

}
AirEngine::Core::Graphic::Command::CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(Graphic::CoreObject::Instance::VkDevice_(), _parentCommandPool->_vkCommandPool, 1, &_vkCommandBuffer);
    vkDestroyFence(Graphic::CoreObject::Instance::VkDevice_(), _vkFence, nullptr);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Reset()
{
    vkResetFences(Graphic::CoreObject::Instance::VkDevice_(), 1, &_vkFence);
    vkResetCommandBuffer(_vkCommandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::BeginRecord(VkCommandBufferUsageFlags flag)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flag;

    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::AddPipelineImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers)
{
    std::vector< VkImageMemoryBarrier> vkBarriers = std::vector< VkImageMemoryBarrier>();
    for (const auto& imageMemoryBarrier : imageMemoryBarriers)
    {
        vkBarriers.insert(vkBarriers.end(), imageMemoryBarrier->VkImageMemoryBarriers().begin(), imageMemoryBarrier->VkImageMemoryBarriers().end());
    }
    vkCmdPipelineBarrier(
        _vkCommandBuffer,
        srcStageMask, dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        static_cast<uint32_t>(vkBarriers.size()), vkBarriers.data()
    );
}
void AirEngine::Core::Graphic::Command::CommandBuffer::AddPipelineBufferBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<BufferMemoryBarrier*> bufferMemoryBarriers)
{
    std::vector< VkBufferMemoryBarrier> vkBufferBarriers = std::vector< VkBufferMemoryBarrier>();
    for (const auto& bufferMemoryBarrier : bufferMemoryBarriers)
    {
        vkBufferBarriers.emplace_back(bufferMemoryBarrier->BufferMemoryBarrier_());
    }
    vkCmdPipelineBarrier(
        _vkCommandBuffer,
        srcStageMask, dstStageMask,
        0,
        0, nullptr,
        static_cast<uint32_t>(vkBufferBarriers.size()), vkBufferBarriers.data(),
        0, nullptr
        );
}
void AirEngine::Core::Graphic::Command::CommandBuffer::AddPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers, std::vector<BufferMemoryBarrier*> bufferMemoryBarriers)
{
    std::vector< VkImageMemoryBarrier> vkImageBarriers = std::vector< VkImageMemoryBarrier>();
    std::vector< VkBufferMemoryBarrier> vkBufferBarriers = std::vector< VkBufferMemoryBarrier>();
    for (const auto& imageMemoryBarrier : imageMemoryBarriers)
    {
        vkImageBarriers.insert(vkImageBarriers.end(), imageMemoryBarrier->VkImageMemoryBarriers().begin(), imageMemoryBarrier->VkImageMemoryBarriers().end());
    }
    for (const auto& bufferMemoryBarrier : bufferMemoryBarriers)
    {
        vkBufferBarriers.emplace_back(bufferMemoryBarrier->BufferMemoryBarrier_());
    }
    vkCmdPipelineBarrier(
        _vkCommandBuffer,
        srcStageMask, dstStageMask,
        0,
        0, nullptr,
        static_cast<uint32_t>(vkBufferBarriers.size()), vkBufferBarriers.data(),
        static_cast<uint32_t>(vkImageBarriers.size()), vkImageBarriers.data()
    );
}
void AirEngine::Core::Graphic::Command::CommandBuffer::CopyImage(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Image* dstImage, VkImageLayout dstImageLayout)
{
    CopyImage(srcImage, "DefaultImageView", srcImageLayout, dstImage, "DefaultImageView", dstImageLayout);
}
void AirEngine::Core::Graphic::Command::CommandBuffer::CopyImage(Instance::Image* srcImage, std::string srcImageViewName, VkImageLayout srcImageLayout, Instance::Image* dstImage, std::string dstImageViewName, VkImageLayout dstImageLayout)
{
    auto& srcImageView = srcImage->ImageView_(srcImageViewName);
    auto& dstImageView = dstImage->ImageView_(dstImageViewName);

    auto aspectFlag = srcImageView.VkImageAspectFlags_();
    auto srcBaseLayer = srcImageView.BaseLayer();
    auto srcLayerCount = srcImageView.LayerCount();
    auto srcMipmapLevel = srcImageView.BaseMipmapLevel();
    auto srcMipmapLevelCount = srcImageView.MipmapLevelCount();
    auto dstBaseLayer = dstImageView.BaseLayer();
    auto dstLayerCount = dstImageView.LayerCount();
    auto dstMipmapLevel = dstImageView.BaseMipmapLevel();
    auto dstMipmapLevelCount = dstImageView.MipmapLevelCount();

    std::vector<VkImageCopy> copys(srcMipmapLevelCount);
    for (int i = 0; i < srcMipmapLevelCount; i++)
    {
        auto& copy = copys[i];
        copy.srcSubresource.aspectMask = aspectFlag;
        copy.srcSubresource.baseArrayLayer = srcBaseLayer;
        copy.srcSubresource.layerCount = srcLayerCount;
        copy.srcSubresource.mipLevel = srcMipmapLevel + i;
        copy.srcOffset = { 0, 0, 0 };
        copy.dstSubresource.aspectMask = aspectFlag;
        copy.dstSubresource.baseArrayLayer = dstBaseLayer;
        copy.dstSubresource.layerCount = dstLayerCount;
        copy.dstSubresource.mipLevel = dstMipmapLevel + i;
        copy.dstOffset = { 0, 0, 0 };
        copy.extent = dstImage->VkExtent3D_();
    }

    vkCmdCopyImage(_vkCommandBuffer, srcImage->VkImage_(), srcImageLayout, dstImage->VkImage_(), dstImageLayout, static_cast<uint32_t>(copys.size()), copys.data());
}
void AirEngine::Core::Graphic::Command::CommandBuffer::AddPipelineImageBarrier(VkDependencyFlags dependencyFlag, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers)
{
    std::vector< VkImageMemoryBarrier> vkBarriers = std::vector< VkImageMemoryBarrier>();
    for (const auto& imageMemoryBarrier : imageMemoryBarriers)
    {
        vkBarriers.insert(vkBarriers.end(), imageMemoryBarrier->VkImageMemoryBarriers().begin(), imageMemoryBarrier->VkImageMemoryBarriers().end());
    }
    vkCmdPipelineBarrier(
        _vkCommandBuffer,
        srcStageMask, dstStageMask,
        dependencyFlag,
        0, nullptr,
        0, nullptr,
        static_cast<uint32_t>(vkBarriers.size()), vkBarriers.data()
    );

}

void AirEngine::Core::Graphic::Command::CommandBuffer::CopyBufferToImage(Instance::Buffer* srcBuffer, Instance::Image* dstImage, VkImageLayout dstImageLayout, uint32_t mipmapLevelOffset)
{
    auto& srcImageView = dstImage->ImageView_();

    auto aspectFlag = srcImageView.VkImageAspectFlags_();
    auto srcBaseLayer = srcImageView.BaseLayer();
    auto srcLayerCount = srcImageView.LayerCount();
    auto srcMipmapLevel = srcImageView.BaseMipmapLevel();

    VkBufferImageCopy copy{};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = aspectFlag;
    copy.imageSubresource.baseArrayLayer = srcBaseLayer;
    copy.imageSubresource.layerCount = srcLayerCount;
    copy.imageSubresource.mipLevel = srcMipmapLevel + mipmapLevelOffset;
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = dstImage->VkExtent3D_();

    vkCmdCopyBufferToImage(_vkCommandBuffer, srcBuffer->VkBuffer_(), dstImage->VkImage_(), dstImageLayout, 1, &copy);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::CopyImageToBuffer(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Buffer* dstBuffer, uint32_t mipmapLevelOffset)
{
    auto& srcImageView = srcImage->ImageView_();

    auto aspectFlag = srcImageView.VkImageAspectFlags_();
    auto srcBaseLayer = srcImageView.BaseLayer();
    auto srcLayerCount = srcImageView.LayerCount();
    auto srcMipmapLevel = srcImageView.BaseMipmapLevel();

    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = aspectFlag;
    copy.imageSubresource.baseArrayLayer = srcBaseLayer;
    copy.imageSubresource.layerCount = srcLayerCount;
    copy.imageSubresource.mipLevel = srcMipmapLevel + mipmapLevelOffset;
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = srcImage->VkExtent3D_();

    vkCmdCopyImageToBuffer(_vkCommandBuffer, srcImage->VkImage_(), srcImageLayout, dstBuffer->VkBuffer_(), 1, &copy);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::FillBuffer(Instance::Buffer* dstBuffer, uint32_t data)
{
    vkCmdFillBuffer(_vkCommandBuffer, dstBuffer->VkBuffer_(), 0, dstBuffer->Size(), data);
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
    Utils::Log::Exception("Failed to end command buffer called: " + _name + ".", vkEndCommandBuffer(_vkCommandBuffer));
}

void AirEngine::Core::Graphic::Command::CommandBuffer::ClearDepthImage(Instance::Image* image, VkImageLayout layout, float depth)
{
    VkClearDepthStencilValue clearValues = { depth, 0 };
    vkCmdClearDepthStencilImage(_vkCommandBuffer, image->VkImage_(), layout, &clearValues, 1, &image->ImageView_().VkImageSubresourceRange_());
}

void AirEngine::Core::Graphic::Command::CommandBuffer::ClearColorImage(Instance::Image* image, VkImageLayout layout, VkClearColorValue targetColor)
{
    vkCmdClearColorImage(_vkCommandBuffer, image->VkImage_(), layout, &targetColor, 1, &image->ImageView_().VkImageSubresourceRange_());
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

void AirEngine::Core::Graphic::Command::CommandBuffer::Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages)
{
    std::vector <VkSemaphore> wait = std::vector <VkSemaphore>(waitSemaphores.size());
    for (uint32_t i = 0; i < waitSemaphores.size(); i++)
    {
        wait[i] = waitSemaphores[i]->VkSemphore_();
    }
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCommandBuffer;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(wait.size());
    submitInfo.pWaitSemaphores = wait.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

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

        Utils::Log::Exception("Failed to submit command buffer called: " + _name + ".", vkQueueSubmit(CoreObject::Instance::Queue_(_parentCommandPool->_queueName)->queue, 1, &submitInfo, _vkFence));
    }
}

void AirEngine::Core::Graphic::Command::CommandBuffer::WaitForFinish()
{
    Utils::Log::Exception("Failed to wait command buffer called: " + _name + ".", vkWaitForFences(Graphic::CoreObject::Instance::VkDevice_(), 1, &_vkFence, VK_TRUE, UINT64_MAX));
}

void AirEngine::Core::Graphic::Command::CommandBuffer::BeginRenderPass(Graphic::Rendering::RenderPassBase* renderPass, Graphic::Rendering::FrameBuffer* frameBuffer, std::map<std::string, VkClearValue> clearValues)
{
    std::vector< VkClearValue> attachmentClearValues = std::vector< VkClearValue>(renderPass->Settings()->attchmentDescriptors.size());
    {
        size_t i = 0;
        for (const auto& attachment : renderPass->Settings()->attchmentDescriptors)
        {
            if (clearValues.count(attachment.first))
            {
                attachmentClearValues[i++] = clearValues[attachment.first];
            }
            else
            {
                attachmentClearValues[i++] = {};
            }
        }
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->VkRenderPass_();
    renderPassInfo.framebuffer = frameBuffer->VkFramebuffer_();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = frameBuffer->Extent2D();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(attachmentClearValues.size());
    renderPassInfo.pClearValues = attachmentClearValues.data();

    _commandData.viewport.x = 0.0f;
    _commandData.viewport.y = static_cast<float>(frameBuffer->Extent2D().height);
    _commandData.viewport.width = static_cast<float>(frameBuffer->Extent2D().width);
    _commandData.viewport.height = -static_cast<float>(frameBuffer->Extent2D().height);
    _commandData.viewport.minDepth = 0.0f;
    _commandData.viewport.maxDepth = 1.0f;

    _commandData.scissor.offset = { 0, 0 };
    _commandData.scissor.extent = frameBuffer->Extent2D();

    vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(_vkCommandBuffer, 0, 1, &_commandData.viewport);
    vkCmdSetScissor(_vkCommandBuffer, 0, 1, &_commandData.scissor);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::BeginRenderPass(Graphic::Rendering::RenderPassBase* renderPass, Graphic::Rendering::FrameBuffer* frameBuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->VkRenderPass_();
    renderPassInfo.framebuffer = frameBuffer->VkFramebuffer_();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = frameBuffer->Extent2D();
    renderPassInfo.clearValueCount = 0;
    renderPassInfo.pClearValues = nullptr;

    _commandData.viewport.x = 0.0f;
    _commandData.viewport.y = static_cast<float>(frameBuffer->Extent2D().height);
    _commandData.viewport.width = static_cast<float>(frameBuffer->Extent2D().width);
    _commandData.viewport.height = -static_cast<float>(frameBuffer->Extent2D().height);
    _commandData.viewport.minDepth = 0.0f;
    _commandData.viewport.maxDepth = 1.0f;

    _commandData.scissor.offset = { 0, 0 };
    _commandData.scissor.extent = frameBuffer->Extent2D();

    vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(_vkCommandBuffer, 0, 1, &_commandData.viewport);
    vkCmdSetScissor(_vkCommandBuffer, 0, 1, &_commandData.scissor);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(_vkCommandBuffer);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::DrawMesh(Asset::Mesh* mesh, Rendering::Material* material)
{
    auto sets = material->VkDescriptorSets();
    if (material->Shader()->ShaderType_() == Rendering::Shader::ShaderType::GRAPHIC)
    {
        vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->Shader()->VkPipeline_());
        vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->PipelineLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
    }
    else if (material->Shader()->ShaderType_() == Rendering::Shader::ShaderType::COMPUTE)
    {
        vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, material->Shader()->VkPipeline_());
        vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, material->PipelineLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
    }
    VkBuffer vertexBuffers[] = { mesh->VertexBuffer().VkBuffer_() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(_vkCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_vkCommandBuffer, mesh->IndexBuffer().VkBuffer_(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(_vkCommandBuffer, static_cast<uint32_t>(mesh->Indices().size()), 1, 0, 0, 0);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Dispatch(Rendering::Material* material, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    auto sets = material->VkDescriptorSets();
    if (material->Shader()->ShaderType_() == Rendering::Shader::ShaderType::GRAPHIC)
    {
        vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->Shader()->VkPipeline_());
        vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->PipelineLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
    }
    else if (material->Shader()->ShaderType_() == Rendering::Shader::ShaderType::COMPUTE)
    {
        vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, material->Shader()->VkPipeline_());
        vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, material->PipelineLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
    }
    vkCmdDispatch(_vkCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

void AirEngine::Core::Graphic::Command::CommandBuffer::Blit(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Image* dstImage, VkImageLayout dstImageLayout, VkFilter filter)
{
    auto& srcImageView = srcImage->ImageView_();
    auto& dstImageView = dstImage->ImageView_();

    auto src = srcImage->VkExtent3D_();
    auto dst = dstImage->VkExtent3D_();
    
    auto aspectFlag = srcImageView.VkImageAspectFlags_();
    auto srcBaseLayer = srcImageView.BaseLayer();
    auto srcLayerCount = srcImageView.LayerCount();
    auto srcMipmapLevel = srcImageView.BaseMipmapLevel();
    auto srcMipmapLevelCount = srcImageView.MipmapLevelCount();
    auto dstBaseLayer = dstImageView.BaseLayer();
    auto dstLayerCount = dstImageView.LayerCount();
    auto dstMipmapLevel = dstImageView.BaseMipmapLevel();
    auto dstMipmapLevelCount = dstImageView.MipmapLevelCount();

    std::vector<VkImageBlit> blits(srcMipmapLevelCount);
    for (int i = 0; i < srcMipmapLevelCount; i++)
    {
        auto& blit = blits[i];
        blit.srcSubresource.aspectMask = aspectFlag;
        blit.srcSubresource.baseArrayLayer = srcBaseLayer;
        blit.srcSubresource.layerCount = srcLayerCount;
        blit.srcSubresource.mipLevel = srcMipmapLevel + i;
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = *reinterpret_cast<VkOffset3D*>(&src);
        blit.dstSubresource.aspectMask = aspectFlag;
        blit.dstSubresource.baseArrayLayer = dstBaseLayer;
        blit.dstSubresource.layerCount = dstLayerCount;
        blit.dstSubresource.mipLevel = dstMipmapLevel + i;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = *reinterpret_cast<VkOffset3D*>(&dst);
    }

    vkCmdBlitImage(_vkCommandBuffer, srcImage->VkImage_(), srcImageLayout, dstImage->VkImage_(), dstImageLayout, static_cast<uint32_t>(blits.size()), blits.data(), filter);
}

AirEngine::Core::Graphic::Command::CommandPool* AirEngine::Core::Graphic::Command::CommandBuffer::ParentCommandPool()
{
    return _parentCommandPool;
}
