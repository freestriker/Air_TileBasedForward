#pragma once
#include <vulkan/vulkan_core.h>
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
			}
			namespace Command
			{
				class ImageMemoryBarrier
				{
					std::vector<VkImageMemoryBarrier> _vkImageMemoryBarriers;
				public:
					ImageMemoryBarrier(Instance::Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags);
					~ImageMemoryBarrier();
					const std::vector<VkImageMemoryBarrier>& VkImageMemoryBarriers();
				};
			}
		}
	}
}
