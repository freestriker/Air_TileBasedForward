#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class NewImage;
			}
			namespace Command
			{
				class ImageMemoryBarrier
				{
					std::vector<VkImageMemoryBarrier> _vkImageMemoryBarriers;
				public:
					ImageMemoryBarrier(Instance::Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags);
					ImageMemoryBarrier(Instance::NewImage* image, std::string imageViewName, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags);
					ImageMemoryBarrier(Instance::NewImage* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags);
					ImageMemoryBarrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags, std::vector<VkImageSubresourceRange> subresourceRanges);
					~ImageMemoryBarrier();
					const std::vector<VkImageMemoryBarrier>& VkImageMemoryBarriers();
				};
			}
		}
	}
}
