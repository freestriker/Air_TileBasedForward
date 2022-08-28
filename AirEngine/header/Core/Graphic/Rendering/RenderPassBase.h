#pragma once
#include <string>
#include <vulkan/vulkan_core.h>
#include <map>
#include <vector>
#include "Core/Logic/Object/Object.h"

namespace AirEngine
{
	namespace Renderer
	{
		class Renderer;
	}
	namespace Camera
	{
		class CameraBase;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Instance;
				class Thread;
			}
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
				class NewRenderPassManager;
				class RenderPassTarget;
			}
			namespace Rendering
			{
				class RenderPassBase: public Core::Logic::Object::Object
				{
					friend class Manager::NewRenderPassManager;
					friend class CoreObject::Instance;
					friend class CoreObject::Thread;
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
					VkRenderPass _vkRenderPass;
					RenderPassSettings _settings;
					std::map<std::string, uint32_t> _subPassIndexMap;
					std::map<std::string, std::map<std::string, uint32_t>> _colorAttachmentIndexMaps;
					std::map<std::string, std::map<std::string, uint32_t>> _depthAttachmentMap;
				protected:
					RenderPassBase();
					virtual ~RenderPassBase();
					RenderPassBase(const RenderPassBase&) = delete;
					RenderPassBase& operator=(const RenderPassBase&) = delete;
					RenderPassBase(RenderPassBase&&) = delete;
					RenderPassBase& operator=(RenderPassBase&&) = delete;

					virtual void OnPopulateRenderPassSettings(RenderPassSettings& creator) = 0;
				public:
					const RenderPassSettings* Settings();
					VkRenderPass VkRenderPass_();
					uint32_t SubPassIndex(std::string subPassName);
					const std::map<std::string, uint32_t>* ColorAttachmentIndexMap(std::string subPassName);
				
					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}
