#pragma once
#include <vulkan/vulkan_core.h>
#include <map>
#include <string>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Manager
			{
				class RenderPassManager;
			}
			namespace RenderPass
			{
				class RenderPassBase;
			}
			namespace Instance
			{
				class Image;
				class FrameBuffer
				{
					friend class Manager::RenderPassManager;
				private:
					VkFramebuffer _vkFrameBuffer;
					std::map<std::string, Image*> _attachments;
				public:
					FrameBuffer(RenderPass::RenderPassBase* renderPass, std::map<std::string, Image*> availableAttachments, VkExtent2D extent);
					~FrameBuffer();
					VkFramebuffer VkFramebuffer_();
					Image* Attachment(std::string name);
				};
			}
		}
	}
}