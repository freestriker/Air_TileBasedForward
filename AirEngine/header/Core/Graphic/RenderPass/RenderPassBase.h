#pragma once
#include <string>
#include <vulkan/vulkan_core.h>
#include <map>
#include <vector>

#define PRE_Z_RENDER_INDEX 0
#define F_OPAQUE_RENDER_INDEX 2000
#define TBF_OPAQUE_RENDER_INDEX 4000
#define BACKGROUND_RENDER_INDEX 6000
#define F_TRANSPARENT_RENDER_INDEX 8000
#define TBF_TRANSPARENT_RENDER_INDEX 10000
#define TBF_OIT_DEPTH_PEELING_RENDER_INDEX 12000
#define TBF_OIT_DEPTH_PEELING_BLEND_RENDER_INDEX 12100
#define TBF_OIT_ALPHA_LINKED_LIST_RENDER_INDEX 14000
#define TBF_OIT_ALL_SORT_BLEND_RENDER_INDEX 14100
#define PRESENT_RENDER_INDEX 2147483647

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
				class RenderPassTarget;
			}
			namespace RenderPass
			{
				class RenderPassBase
				{
					friend class Manager::RenderPassManager;
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
					RenderPassBase(const RenderPassBase&) = delete;
					RenderPassBase& operator=(const RenderPassBase&) = delete;
					RenderPassBase(RenderPassBase&&) = delete;
					RenderPassBase& operator=(RenderPassBase&&) = delete;
					std::map<std::string, Command::Semaphore*> _customSemaphores;

					virtual void OnPopulateRenderPassSettings(RenderPassSettings& creator) = 0;
					virtual void OnPrepare(Camera::CameraBase* camera);
					virtual void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera) = 0;
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
