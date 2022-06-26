#pragma once
#include <string>
#include <vulkan/vulkan_core.h>
#include <map>
#include <vector>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class FrameBuffer;
			}
			namespace Command
			{
				class CommandPool;
				class Semaphore;
			}
			namespace Manager
			{
				class RenderPassManager;
			}
			namespace RenderPass
			{
				class RenderPassBase
				{
					friend class Manager::RenderPassManager;
				public:
					class RenderPassSettings final
					{
					public:
						struct AttachmentDescriptor
						{
							std::string name;
							VkFormat format;
							VkSampleCountFlags sampleCount;
							VkAttachmentLoadOp loadOp;
							VkAttachmentStoreOp storeOp;
							VkAttachmentLoadOp stencilLoadOp;
							VkAttachmentStoreOp stencilStoreOp;
							VkImageLayout initialLayout;
							VkImageLayout layout;
							VkImageLayout finalLayout;
							bool useStencil;
							AttachmentDescriptor(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkImageLayout layout, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout, bool isStencil, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp);
						};
						struct SubpassDescriptor
						{
							std::string name;
							VkPipelineBindPoint pipelineBindPoint;
							std::vector<std::string> colorAttachmentNames;
							std::string depthStencilAttachmentName;
							bool useDepthStencilAttachment;

							SubpassDescriptor(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, bool useDepthStencilAttachment, std::string depthStencilAttachmentName);
						};
						struct DependencyDescriptor
						{
							std::string srcSubpassName;
							std::string dstSubpassName;
							VkPipelineStageFlags srcStageMask;
							VkPipelineStageFlags dstStageMask;
							VkAccessFlags srcAccessMask;
							VkAccessFlags dstAccessMask;
							DependencyDescriptor(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);
						};
						std::map <std::string, AttachmentDescriptor> attchmentDescriptors;
						std::map <std::string, SubpassDescriptor> subpassDescriptors;
						std::vector <DependencyDescriptor> dependencyDescriptors;
						RenderPassSettings();
						void AddColorAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout);
						void AddDepthAttachment(std::string name, VkFormat format, VkSampleCountFlags sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout);
						void AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames);
						void AddSubpass(std::string name, VkPipelineBindPoint pipelineBindPoint, std::vector<std::string> colorAttachmentNames, std::string depthAttachmentName);
						void AddDependency(std::string srcSubpassName, std::string dstSubpassName, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);
					};
				private:
					std::string _name;
					uint32_t _renderIndex;
					VkRenderPass _vkRenderPass;
					RenderPassSettings _settings;
					std::map<std::string, uint32_t> _subPassMap;
					std::map<std::string, std::map<std::string, uint32_t>> _colorAttachmentMap;
					std::map<std::string, std::map<std::string, uint32_t>> _depthAttachmentMap;
					Command::Semaphore* _semaphore;
					void CreateRenderPass();
				protected:
					RenderPassBase(std::string name, uint32_t renderIndex);
					virtual ~RenderPassBase();
					std::map<std::string, Command::Semaphore*> _customSemaphores;

					virtual void OnPopulateRenderPassSettings(RenderPassSettings& creator) = 0;
					virtual void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, void*>& renderDistanceTable, std::map<std::string, Instance::Image*> attachemnts) = 0;
					virtual void OnSubmit() = 0;
					virtual void OnClear() = 0;
				public:
					std::string Name();
					const RenderPassSettings& Settings();
					uint32_t RenderIndex();
					VkRenderPass VkRenderPass_();
					Command::Semaphore* Semaphore();
					Command::Semaphore* CustomSemaphore(std::string name);
					uint32_t SubPassIndex(std::string subPassName);
					uint32_t ColorAttachmentIndex(std::string subPassName, std::string attachmentName);
					std::vector<std::string> SubPassNames();
					std::vector<std::string> ColorAttachmentNames(std::string subPassName);
					std::map<std::string, uint32_t> ColorAttachmentMap(std::string subPassName);
					std::map<std::string, std::map<std::string, uint32_t>> ColorAttachmentMaps();
				};
			}
		}
	}
}
