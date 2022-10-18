#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/Image.h"
#include <string>

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(Instance::Image* image, std::string imageViewName, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
{
	_vkImageMemoryBarriers.push_back({});
	auto& barrier = _vkImageMemoryBarriers.back();
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->VkImage_();
	barrier.subresourceRange = image->ImageView_(imageViewName).vkImageSubresourceRange;
	barrier.srcAccessMask = srcAccessFlags;
	barrier.dstAccessMask = dstAccessFlags;
}

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(Instance::Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
{
	_vkImageMemoryBarriers.push_back({});
	auto& barrier = _vkImageMemoryBarriers.back();
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->VkImage_();
	barrier.subresourceRange = image->ImageView_().vkImageSubresourceRange;
	barrier.srcAccessMask = srcAccessFlags;
	barrier.dstAccessMask = dstAccessFlags;
}

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::~ImageMemoryBarrier()
{
}

const std::vector<VkImageMemoryBarrier>& AirEngine::Core::Graphic::Command::ImageMemoryBarrier::VkImageMemoryBarriers()
{
	return _vkImageMemoryBarriers;
}