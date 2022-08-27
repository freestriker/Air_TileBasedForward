#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Command/Semaphore.h"
#include "Core/Graphic/Manager/RenderPassManager.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<AirEngine::Core::Graphic::Rendering::RenderPassBase>("AirEngine::Core::Graphic::Rendering::RenderPassBase");
}


AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AttachmentDescriptor::AttachmentDescriptor(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkImageLayout layout, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout, bool isStencil, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp)
    : name(name)
    , format(format)
    , sampleCount(sampleCount)
    , layout(layout)
    , loadOp(loadOp)
    , storeOp(storeOp)
    , stencilLoadOp(stencilLoadOp)
    , stencilStoreOp(stencilStoreOp)
    , initialLayout(initialLayout)
    , finalLayout(finalLayout)
    , useStencil(isStencil)
{
}

AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::SubpassDescriptor::SubpassDescriptor(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, bool useDepthStencilAttachment, std::string depthStencilAttachmentName)
    : name(name)
    , pipelineBindPoint(pipelineBindPoint)
    , colorAttachmentNames(colorAttachmentNames)
    , useDepthStencilAttachment(useDepthStencilAttachment)
    , depthStencilAttachmentName(depthStencilAttachmentName)
{
}

AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::DependencyDescriptor::DependencyDescriptor(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
    : srcSubpassName(srcSubpassName)
    , dstSubpassName(dstSubpassName)
    , srcStageMask(srcStageMask)
    , dstStageMask(dstStageMask)
    , srcAccessMask(srcAccessMask)
    , dstAccessMask(dstAccessMask)
{
}

AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::RenderPassSettings()
    : attchmentDescriptors()
    , subpassDescriptors()
    , dependencyDescriptors()
{
}

void AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AddColorAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    attchmentDescriptors.insert({ name, AttachmentDescriptor(name, format, sampleCount, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, loadOp, storeOp, initialLayout, finalLayout, false, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE) });
}

void AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AddDepthAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    attchmentDescriptors.insert({ name, AttachmentDescriptor(name, format, sampleCount, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, loadOp, storeOp, initialLayout, finalLayout, false, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE) });
}

void AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames)
{
    subpassDescriptors.insert({ name, SubpassDescriptor(name, pipelineBindPoint, colorAttachmentNames, false, "") });
}

void AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, std::string depthAttachmentName)
{
    subpassDescriptors.insert({ name, SubpassDescriptor(name, pipelineBindPoint, colorAttachmentNames, true, depthAttachmentName) });
}

void AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassSettings::AddDependency(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
{
    dependencyDescriptors.push_back(DependencyDescriptor(srcSubpassName, dstSubpassName, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask));
}

AirEngine::Core::Graphic::Rendering::RenderPassBase::RenderPassBase()
    : _vkRenderPass(VK_NULL_HANDLE)
    , _subPassIndexMap()
    , _colorAttachmentIndexMaps()
{

}

AirEngine::Core::Graphic::Rendering::RenderPassBase::~RenderPassBase()
{
    vkDestroyRenderPass(CoreObject::Instance::VkDevice_(), _vkRenderPass, nullptr);
}

VkRenderPass AirEngine::Core::Graphic::Rendering::RenderPassBase::VkRenderPass_()
{
    return _vkRenderPass;
}

uint32_t AirEngine::Core::Graphic::Rendering::RenderPassBase::SubPassIndex(std::string subPassName)
{
    return _subPassIndexMap[subPassName];
}

const std::map<std::string, uint32_t>* AirEngine::Core::Graphic::Rendering::RenderPassBase::ColorAttachmentIndexMap(std::string subPassName)
{
    return &_colorAttachmentIndexMaps[subPassName];
}
