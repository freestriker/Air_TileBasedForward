#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Command/Semaphore.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AttachmentDescriptor::AttachmentDescriptor(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkImageLayout layout, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout, bool isStencil, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp)
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

AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::SubpassDescriptor::SubpassDescriptor(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, bool useDepthStencilAttachment, std::string depthStencilAttachmentName)
    : name(name)
    , pipelineBindPoint(pipelineBindPoint)
    , colorAttachmentNames(colorAttachmentNames)
    , useDepthStencilAttachment(useDepthStencilAttachment)
    , depthStencilAttachmentName(depthStencilAttachmentName)
{
}

AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::DependencyDescriptor::DependencyDescriptor(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
    : srcSubpassName(srcSubpassName)
    , dstSubpassName(dstSubpassName)
    , srcStageMask(srcStageMask)
    , dstStageMask(dstStageMask)
    , srcAccessMask(srcAccessMask)
    , dstAccessMask(dstAccessMask)
{
}

AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::RenderPassSettings()
    : attchmentDescriptors()
    , subpassDescriptors()
    , dependencyDescriptors()
{
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AddColorAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    attchmentDescriptors.insert({ name, AttachmentDescriptor(name, format, sampleCount, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, loadOp, storeOp, initialLayout, finalLayout, false, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE) });
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AddDepthAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    attchmentDescriptors.insert({ name, AttachmentDescriptor(name, format, sampleCount, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, loadOp, storeOp, initialLayout, finalLayout, false, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE) });
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames)
{
    subpassDescriptors.insert({ name, SubpassDescriptor(name, pipelineBindPoint, colorAttachmentNames, false, "") });
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, std::string depthAttachmentName)
{
    subpassDescriptors.insert({ name, SubpassDescriptor(name, pipelineBindPoint, colorAttachmentNames, true, depthAttachmentName) });
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings::AddDependency(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
{
    dependencyDescriptors.push_back(DependencyDescriptor(srcSubpassName, dstSubpassName, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask));
}

void AirEngine::Core::Graphic::RenderPass::RenderPassBase::CreateRenderPass()
{
    OnPopulateRenderPassSettings(_settings);

    std::map<std::string, uint32_t> attachmentIndexes;
    std::vector<VkAttachmentDescription> attachments = std::vector<VkAttachmentDescription>(_settings.attchmentDescriptors.size());
    std::vector<VkAttachmentReference> attachmentReferences = std::vector<VkAttachmentReference>(_settings.attchmentDescriptors.size());
    {
        uint32_t attachmentIndex = 0;
        for (const auto& pair : _settings.attchmentDescriptors)
        {
            const auto& attachmentDescriptor = pair.second;

            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = attachmentDescriptor.format;
            colorAttachment.samples = static_cast<VkSampleCountFlagBits>(attachmentDescriptor.sampleCount);
            colorAttachment.loadOp = attachmentDescriptor.loadOp;
            colorAttachment.storeOp = attachmentDescriptor.storeOp;
            colorAttachment.stencilLoadOp = attachmentDescriptor.stencilLoadOp;
            colorAttachment.stencilStoreOp = attachmentDescriptor.stencilStoreOp;
            colorAttachment.initialLayout = attachmentDescriptor.initialLayout;
            colorAttachment.finalLayout = attachmentDescriptor.finalLayout;

            VkAttachmentReference attachmentReference{};
            attachmentReference.attachment = attachmentIndex;
            attachmentReference.layout = attachmentDescriptor.layout;

            attachments[attachmentIndex] = colorAttachment;
            attachmentReferences[attachmentIndex] = attachmentReference;
            attachmentIndexes[attachmentDescriptor.name] = attachmentIndex;

            ++attachmentIndex;
        }
    }

    std::map<std::string, uint32_t> subpassMap;
    std::map<std::string, std::map<std::string, uint32_t>> colorAttachmentMap;
    std::vector<VkSubpassDescription> subpasss = std::vector<VkSubpassDescription>(_settings.subpassDescriptors.size());
    std::vector<std::vector<VkAttachmentReference>> colorAttachments = std::vector<std::vector<VkAttachmentReference>>(_settings.subpassDescriptors.size());
    std::vector<VkAttachmentReference> depthStencilAttachments = std::vector<VkAttachmentReference>(_settings.subpassDescriptors.size());
    {
        uint32_t subpassIndex = 0;
        for (const auto& pair : _settings.subpassDescriptors)
        {
            const auto& subpassDescriptor = pair.second;

            subpassMap[subpassDescriptor.name] = subpassIndex;
            colorAttachmentMap[subpassDescriptor.name] = std::map<std::string, uint32_t>();

            colorAttachments[subpassIndex].resize(subpassDescriptor.colorAttachmentNames.size());
            for (uint32_t i = 0; i < subpassDescriptor.colorAttachmentNames.size(); i++)
            {
                colorAttachments[subpassIndex][i] = attachmentReferences[attachmentIndexes[subpassDescriptor.colorAttachmentNames[i]]];
                colorAttachmentMap[subpassDescriptor.name][subpassDescriptor.colorAttachmentNames[i]] = i;
            }

            if (subpassDescriptor.useDepthStencilAttachment)
            {
                depthStencilAttachments[subpassIndex] = attachmentReferences[attachmentIndexes[subpassDescriptor.depthStencilAttachmentName]];
            }


            ++subpassIndex;
        }

        for (const auto& pair : _settings.subpassDescriptors)
        {
            const auto& subpassDescriptor = pair.second;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = subpassDescriptor.pipelineBindPoint;
            subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments[subpassMap[subpassDescriptor.name]].size());
            subpass.pColorAttachments = colorAttachments[subpassMap[subpassDescriptor.name]].data();
            if (subpassDescriptor.useDepthStencilAttachment)
            {
                subpass.pDepthStencilAttachment = &(depthStencilAttachments[subpassMap[subpassDescriptor.name]]);
            }

            subpasss[subpassMap[subpassDescriptor.name]] = subpass;
        }
    }

    std::vector< VkSubpassDependency> dependencys = std::vector< VkSubpassDependency>(_settings.dependencyDescriptors.size());
    uint32_t dependencyIndex = 0;
    for (const auto& dependencyDescriptor : _settings.dependencyDescriptors)
    {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = dependencyDescriptor.srcSubpassName == "VK_SUBPASS_EXTERNAL" ? VK_SUBPASS_EXTERNAL : subpassMap[dependencyDescriptor.srcSubpassName];
        dependency.dstSubpass = subpassMap[dependencyDescriptor.dstSubpassName];
        dependency.srcStageMask = dependencyDescriptor.srcStageMask;
        dependency.srcAccessMask = dependencyDescriptor.srcAccessMask;
        dependency.dstStageMask = dependencyDescriptor.dstStageMask;
        dependency.dstAccessMask = dependencyDescriptor.dstAccessMask;
        dependency.dependencyFlags = dependency.srcSubpass == dependency.dstSubpass ? VK_DEPENDENCY_BY_REGION_BIT : 0;

        dependencys[dependencyIndex] = dependency;

        ++dependencyIndex;
    }

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subpasss.size());
    renderPassInfo.pSubpasses = subpasss.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencys.size());
    renderPassInfo.pDependencies = dependencys.data();

    VkRenderPass newVkRenderPass{};
    Utils::Log::Exception("Failed to create render pass", vkCreateRenderPass(CoreObject::Instance::VkDevice_(), &renderPassInfo, nullptr, &newVkRenderPass));

    _vkRenderPass = newVkRenderPass;
    _subPassMap = subpassMap;
    _colorAttachmentMap = colorAttachmentMap;
}

AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassBase(std::string name, uint32_t renderIndex)
    : _name(name)
    , _renderIndex(renderIndex)
    , _vkRenderPass(VK_NULL_HANDLE)
    , _subPassMap()
    , _colorAttachmentMap()
    , _semaphore(new Command::Semaphore())
    , _customSemaphores()
{

}

AirEngine::Core::Graphic::RenderPass::RenderPassBase::~RenderPassBase()
{
    vkDestroyRenderPass(CoreObject::Instance::VkDevice_(), _vkRenderPass, nullptr);
}

std::string AirEngine::Core::Graphic::RenderPass::RenderPassBase::Name()
{
    return _name;
}

const AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderPassSettings& AirEngine::Core::Graphic::RenderPass::RenderPassBase::Settings()
{
    return _settings;
}

uint32_t AirEngine::Core::Graphic::RenderPass::RenderPassBase::RenderIndex()
{
    return _renderIndex;
}

VkRenderPass AirEngine::Core::Graphic::RenderPass::RenderPassBase::VkRenderPass_()
{
    return _vkRenderPass;
}

AirEngine::Core::Graphic::Command::Semaphore* AirEngine::Core::Graphic::RenderPass::RenderPassBase::Semaphore()
{
    return _semaphore;
}

AirEngine::Core::Graphic::Command::Semaphore* AirEngine::Core::Graphic::RenderPass::RenderPassBase::CustomSemaphore(std::string name)
{
    return _customSemaphores[name];
}

uint32_t AirEngine::Core::Graphic::RenderPass::RenderPassBase::SubPassIndex(std::string subPassName)
{
    return _subPassMap[subPassName];
}

uint32_t AirEngine::Core::Graphic::RenderPass::RenderPassBase::ColorAttachmentIndex(std::string subPassName, std::string attachmentName)
{
    return _colorAttachmentMap[subPassName][attachmentName];
}

std::vector<std::string> AirEngine::Core::Graphic::RenderPass::RenderPassBase::SubPassNames()
{
    std::vector<std::string> names = std::vector<std::string>(_subPassMap.size());
    for (const auto& pair : _subPassMap)
    {
        names[pair.second] = pair.first;
    }
    return names;
}

std::vector<std::string> AirEngine::Core::Graphic::RenderPass::RenderPassBase::ColorAttachmentNames(std::string subPassName)
{
    std::vector<std::string> names = std::vector<std::string>(_colorAttachmentMap[subPassName].size());
    for (const auto& pair : _colorAttachmentMap[subPassName])
    {
        names[pair.second] = pair.first;
    }
    return names;
}

std::map<std::string, uint32_t> AirEngine::Core::Graphic::RenderPass::RenderPassBase::ColorAttachmentMap(std::string subPassName)
{
    return _colorAttachmentMap[subPassName];
}

std::map<std::string, std::map<std::string, uint32_t>> AirEngine::Core::Graphic::RenderPass::RenderPassBase::ColorAttachmentMaps()
{
    return _colorAttachmentMap;
}