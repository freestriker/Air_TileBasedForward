#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/Instance.h"

AirEngine::Core::Graphic::Instance::FrameBuffer::FrameBuffer(RenderPass::RenderPassBase* renderPass, std::map<std::string, Image*> availableAttachments, VkExtent2D extent)
    : _vkFrameBuffer(VK_NULL_HANDLE)
    , _attachments()
{
    std::vector<VkImageView> views = std::vector<VkImageView>();
    for (const auto& descriptorPair : renderPass->Settings().attchmentDescriptors)
    {
        const auto& descriptor = descriptorPair.second;
        auto targetIter = availableAttachments.find(descriptorPair.first);
        if (targetIter == availableAttachments.end()) Utils::Log::Exception("The available attachment do not match the render pass.");
        auto targetAttachment = targetIter->second;
        if(descriptor.format != targetAttachment->VkFormat_()) Utils::Log::Exception("The available attachment's format do not match the render pass.");
        views.emplace_back(targetAttachment->VkImageView_());
        _attachments.emplace(descriptorPair.first, targetAttachment);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->VkRenderPass_();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(views.size());
    framebufferInfo.pAttachments = views.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    Utils::Log::Exception("Failed to create framebuffer.", vkCreateFramebuffer(CoreObject::Instance::VkDevice_(), &framebufferInfo, nullptr, &_vkFrameBuffer));

}
AirEngine::Core::Graphic::Instance::FrameBuffer::~FrameBuffer()
{
    vkDestroyFramebuffer(CoreObject::Instance::VkDevice_(), _vkFrameBuffer, nullptr);
}

VkFramebuffer AirEngine::Core::Graphic::Instance::FrameBuffer::VkFramebuffer_()
{
	return _vkFrameBuffer;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::FrameBuffer::Attachment(std::string name)
{
	return _attachments[name];
}
