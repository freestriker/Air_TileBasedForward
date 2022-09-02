#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include <map>
#include <string>
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"

AirEngine::Core::Graphic::Manager::RenderPassManager::RenderPassManager()
    : _managerMutex()
    , _renderPassWrappers()
{
}

AirEngine::Core::Graphic::Manager::RenderPassManager::~RenderPassManager()
{
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::CreateRenderPass(Rendering::RenderPassBase* renderPass)
{
    renderPass->OnPopulateRenderPassSettings(renderPass->_settings);

    std::map<std::string, uint32_t> attachmentIndexes;
    std::vector<VkAttachmentDescription> attachments = std::vector<VkAttachmentDescription>(renderPass->_settings.attchmentDescriptors.size());
    std::vector<VkAttachmentReference> attachmentReferences = std::vector<VkAttachmentReference>(renderPass->_settings.attchmentDescriptors.size());
    {
        uint32_t attachmentIndex = 0;
        for (const auto& pair : renderPass->_settings.attchmentDescriptors)
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
    std::vector<VkSubpassDescription> subpasss = std::vector<VkSubpassDescription>(renderPass->_settings.subpassDescriptors.size());
    std::vector<std::vector<VkAttachmentReference>> colorAttachments = std::vector<std::vector<VkAttachmentReference>>(renderPass->_settings.subpassDescriptors.size());
    std::vector<VkAttachmentReference> depthStencilAttachments = std::vector<VkAttachmentReference>(renderPass->_settings.subpassDescriptors.size());
    {
        uint32_t subpassIndex = 0;
        for (const auto& pair : renderPass->_settings.subpassDescriptors)
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

        for (const auto& pair : renderPass->_settings.subpassDescriptors)
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

    std::vector< VkSubpassDependency> dependencys = std::vector< VkSubpassDependency>(renderPass->_settings.dependencyDescriptors.size());
    uint32_t dependencyIndex = 0;
    for (const auto& dependencyDescriptor : renderPass->_settings.dependencyDescriptors)
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

    renderPass->_vkRenderPass = newVkRenderPass;
    renderPass->_subPassIndexMap = subpassMap;
    renderPass->_colorAttachmentIndexMaps = colorAttachmentMap;
}

AirEngine::Core::Graphic::Rendering::RenderPassBase* AirEngine::Core::Graphic::Manager::RenderPassManager::LoadRenderPass(std::string renderPassName)
{
    std::lock_guard<std::mutex> locker(_managerMutex);

    auto iterator = _renderPassWrappers.find(renderPassName);
    if (iterator == std::end(_renderPassWrappers))
    {
        rttr::type renderPassType = rttr::type::get_by_name(renderPassName);
        Utils::Log::Exception("Do not have render pass type named: " + renderPassName + ".", !renderPassType.is_valid());

        rttr::variant renderPassVariant = renderPassType.create();
        Utils::Log::Exception("Can not create render pass variant named: " + renderPassName + ".", !renderPassVariant.is_valid());

        AirEngine::Core::Graphic::Rendering::RenderPassBase* renderPass = renderPassVariant.get_value<AirEngine::Core::Graphic::Rendering::RenderPassBase*>();
        Utils::Log::Exception("Can not cast render pass named: " + renderPassName + ".", renderPass == nullptr);

        CreateRenderPass(renderPass);

        _renderPassWrappers.emplace(renderPassName, RenderPassWrapper{ 0, renderPass });
        iterator = _renderPassWrappers.find(renderPassName);
    }
    iterator->second.refrenceCount++;
    return iterator->second.renderPass;
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::UnloadRenderPass(std::string renderPassName)
{
    std::lock_guard<std::mutex> locker(_managerMutex);

    auto iterator = _renderPassWrappers.find(renderPassName);
    Utils::Log::Exception("Do not have render pass instance named: " + renderPassName + ".", iterator == std::end(_renderPassWrappers));
    iterator->second.refrenceCount--;
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::UnloadRenderPass(Rendering::RenderPassBase* renderPass)
{
    std::lock_guard<std::mutex> locker(_managerMutex);
    
    std::string renderPassName = renderPass->Type().get_name().to_string();

    auto iterator = _renderPassWrappers.find(renderPassName);
    Utils::Log::Exception("Do not have render pass instance named: " + renderPassName + ".", iterator == std::end(_renderPassWrappers));
    iterator->second.refrenceCount--;
}

void AirEngine::Core::Graphic::Manager::RenderPassManager::Collect()
{
    std::lock_guard<std::mutex> locker(_managerMutex);

    for (auto iterator = _renderPassWrappers.begin(); iterator != _renderPassWrappers.end(); )
    {
        if (iterator->second.refrenceCount == 0)
        {
            delete iterator->second.renderPass;
            iterator = _renderPassWrappers.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }
}
