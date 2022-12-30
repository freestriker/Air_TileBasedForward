#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include <algorithm>

AirEngine::Core::Graphic::Rendering::FrameBuffer::FrameBuffer(Rendering::RenderPassBase* renderPass, std::map<std::string, AirEngine::Core::Graphic::Instance::Image*> availableAttachments, std::map<std::string, std::string> availableAttachmentViews)
    : _vkFrameBuffer(VK_NULL_HANDLE)
    , _attachments()
    , _extent2D()
{
    std::vector<VkImageView> views = std::vector<VkImageView>();
    for (const auto& descriptorPair : renderPass->Settings()->attchmentDescriptors)
    {
        const auto& descriptor = descriptorPair.second;
        auto targetIter = availableAttachments.find(descriptorPair.first);
        Utils::Log::Exception("The available attachment do not match the render pass.", targetIter == availableAttachments.end());
        auto targetAttachment = targetIter->second;
        Utils::Log::Exception("The available attachment's format do not match the render pass.", descriptor.format != targetAttachment->VkFormat_());
        auto viewIter = availableAttachmentViews.find(descriptorPair.first);
        auto& targetImageView = targetAttachment->ImageView_(viewIter == availableAttachmentViews.end() ? "DefaultImageView" : viewIter->second);
        views.emplace_back(targetImageView.VkImageView_());
        _attachments.emplace(descriptorPair.first, targetAttachment);

        auto atrgetImageViewExtent2D = targetAttachment->VkExtent2D_(targetImageView.BaseMipmapLevel());
        _extent2D.width = std::max(_extent2D.width, atrgetImageViewExtent2D.width);
        _extent2D.height = std::max(_extent2D.height, atrgetImageViewExtent2D.height);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->VkRenderPass_();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(views.size());
    framebufferInfo.pAttachments = views.data();
    framebufferInfo.width = _extent2D.width;
    framebufferInfo.height = _extent2D.height;
    framebufferInfo.layers = 1;

    Utils::Log::Exception("Failed to create framebuffer.", vkCreateFramebuffer(CoreObject::Instance::VkDevice_(), &framebufferInfo, nullptr, &_vkFrameBuffer));
}

AirEngine::Core::Graphic::Rendering::FrameBuffer::~FrameBuffer()
{
    vkDestroyFramebuffer(CoreObject::Instance::VkDevice_(), _vkFrameBuffer, nullptr);
}

VkFramebuffer AirEngine::Core::Graphic::Rendering::FrameBuffer::VkFramebuffer_()
{
	return _vkFrameBuffer;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Rendering::FrameBuffer::Attachment(std::string name)
{
	return _attachments[name];
}

VkExtent2D AirEngine::Core::Graphic::Rendering::FrameBuffer::Extent2D()
{
    return _extent2D;
}
