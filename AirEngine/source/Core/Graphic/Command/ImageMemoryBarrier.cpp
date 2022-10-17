#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/NewImage.h"
#include <string>

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(Instance::Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
	: _vkImageMemoryBarriers()
{
	auto layerCount = image->LayerCount();
	auto ranges = image->VkImageSubresourceRanges_();

	_vkImageMemoryBarriers.resize(layerCount);

	for (uint32_t i = 0; i < layerCount; i++)
	{
		auto& _vkImageMemoryBarrier = _vkImageMemoryBarriers[i];

		_vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_vkImageMemoryBarrier.oldLayout = oldLayout;
		_vkImageMemoryBarrier.newLayout = newLayout;
		_vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_vkImageMemoryBarrier.image = image->VkImage_();
		_vkImageMemoryBarrier.subresourceRange = ranges[i];
		_vkImageMemoryBarrier.srcAccessMask = srcAccessFlags;
		_vkImageMemoryBarrier.dstAccessMask = dstAccessFlags;
	}
}

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(Instance::NewImage* image, std::string imageViewName, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
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

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(Instance::NewImage* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
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

AirEngine::Core::Graphic::Command::ImageMemoryBarrier::ImageMemoryBarrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags, std::vector<VkImageSubresourceRange> subresourceRanges)
	: _vkImageMemoryBarriers()
{
	_vkImageMemoryBarriers.resize(subresourceRanges.size());

	for (uint32_t i = 0; i < _vkImageMemoryBarriers.size(); i++)
	{
		auto& _vkImageMemoryBarrier = _vkImageMemoryBarriers[i];

		_vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_vkImageMemoryBarrier.oldLayout = oldLayout;
		_vkImageMemoryBarrier.newLayout = newLayout;
		_vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_vkImageMemoryBarrier.image = image;
		_vkImageMemoryBarrier.subresourceRange = subresourceRanges[i];
		_vkImageMemoryBarrier.srcAccessMask = srcAccessFlags;
		_vkImageMemoryBarrier.dstAccessMask = dstAccessFlags;
	}
}


AirEngine::Core::Graphic::Command::ImageMemoryBarrier::~ImageMemoryBarrier()
{
}

const std::vector<VkImageMemoryBarrier>& AirEngine::Core::Graphic::Command::ImageMemoryBarrier::VkImageMemoryBarriers()
{
	return _vkImageMemoryBarriers;
}