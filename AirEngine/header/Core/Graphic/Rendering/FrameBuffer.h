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
			namespace Instance
			{
				class Image;
			}
			namespace Rendering
			{
				class RenderPassBase;
				class FrameBuffer final
				{
					friend class Manager::RenderPassManager;
				private:
					VkFramebuffer _vkFrameBuffer;
					std::map<std::string, Instance::Image*> _attachments;
					VkExtent2D _extent2D;
				public:
					FrameBuffer(Rendering::RenderPassBase* renderPass, std::map<std::string, Instance::Image*> availableAttachments, std::map<std::string, std::string> availableAttachmentViews = {});
					~FrameBuffer();
					FrameBuffer(const FrameBuffer&) = delete;
					FrameBuffer& operator=(const FrameBuffer&) = delete;
					FrameBuffer(FrameBuffer&&) = delete;
					FrameBuffer& operator=(FrameBuffer&&) = delete;
					VkFramebuffer VkFramebuffer_();
					Instance::Image* Attachment(std::string name);
					VkExtent2D Extent2D();
				};
			}
		}
	}
}