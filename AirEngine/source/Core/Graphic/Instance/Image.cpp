#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Instance/Memory.h"
#include "Core/Graphic/Manager/MemoryManager.h"

AirEngine::Core::Graphic::Instance::Image::Image()
	: _vkImageType()
	, _extent()
	, _vkFormat()
	, _vkImageTiling()
	, _vkImageUsage()
	, _mipLevels()
	, _vkSampleCount()
	, _vkMemoryProperty()
	, _vkImageAspect()
	, _vkImage(VK_NULL_HANDLE)
	, _vkImageViewType()
	, _vkImageView(VK_NULL_HANDLE)
	, _memory(nullptr)
	, _layerCount(1)
	, _perLayerSize()
	, _isNative(false)
{

}


AirEngine::Core::Graphic::Instance::Image::~Image()
{
	if (!_isNative)
	{
		vkDestroyImageView(Core::Graphic::CoreObject::Instance::VkDevice_(), _vkImageView, nullptr);
		vkDestroyImage(Core::Graphic::CoreObject::Instance::VkDevice_(), _vkImage, nullptr);
		Core::Graphic::CoreObject::Instance::MemoryManager().ReleaseMemory(*_memory);
		delete _memory;
	}
}

VkImage AirEngine::Core::Graphic::Instance::Image::VkImage_()
{
	return _vkImage;
}

VkImageView AirEngine::Core::Graphic::Instance::Image::VkImageView_()
{
	return _vkImageView;
}

VkExtent3D AirEngine::Core::Graphic::Instance::Image::VkExtent3D_()
{
	return _extent;
}

AirEngine::Core::Graphic::Instance::Memory& AirEngine::Core::Graphic::Instance::Image::Memory_()
{
	return *_memory;
}

VkFormat AirEngine::Core::Graphic::Instance::Image::VkFormat_()
{
	return _vkFormat;
}

VkSampleCountFlags AirEngine::Core::Graphic::Instance::Image::VkSampleCountFlags_()
{
	return _vkSampleCount;
}

std::vector<VkImageSubresourceRange> AirEngine::Core::Graphic::Instance::Image::VkImageSubresourceRanges_()
{
	std::vector<VkImageSubresourceRange> targets = std::vector<VkImageSubresourceRange>(_layerCount);
	for (uint32_t i = 0; i < _layerCount; i++)
	{
		auto& range = targets[i];

		range.aspectMask = _vkImageAspect;
		range.baseMipLevel = 0;
		range.levelCount = _mipLevels;
		range.baseArrayLayer = static_cast<uint32_t>(i);
		range.layerCount = 1;
	}

	return targets;
}

std::vector<VkImageSubresourceLayers> AirEngine::Core::Graphic::Instance::Image::VkImageSubresourceLayers_()
{
	std::vector<VkImageSubresourceLayers> targets = std::vector<VkImageSubresourceLayers>(_layerCount);
	for (uint32_t i = 0; i < _layerCount; i++)
	{
		auto& layer = targets[i];

		layer.aspectMask = _vkImageAspect;
		layer.mipLevel = 0;
		layer.baseArrayLayer = static_cast<uint32_t>(i);
		layer.layerCount = 1;
	}
	return targets;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::LayerCount()
{
	return _layerCount;
}

size_t AirEngine::Core::Graphic::Instance::Image::PerLayerSize()
{
	return _perLayerSize;
}

VkImageUsageFlags AirEngine::Core::Graphic::Instance::Image::VkImageUsageFlags_()
{
	return _vkImageUsage;
}

VkMemoryPropertyFlags AirEngine::Core::Graphic::Instance::Image::VkMemoryPropertyFlags_()
{
	return _vkMemoryProperty;
}

VkImageAspectFlags AirEngine::Core::Graphic::Instance::Image::VkImageAspectFlags_()
{
	return _vkImageAspect;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::CreateCubeImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	imageInfo.extent = { extent.width, extent.height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 6;
	imageInfo.format = format;
	imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = imageUsage;
	imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	VkImage newVkImage = VK_NULL_HANDLE;
	Utils::Log::Exception("Failed to create image.", vkCreateImage(CoreObject::Instance::VkDevice_(), &imageInfo, nullptr, &newVkImage));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(CoreObject::Instance::VkDevice_(), newVkImage, &memRequirements);

	auto newMemory = new Instance::Memory(CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, memoryProperty));
	vkBindImageMemory(CoreObject::Instance::VkDevice_(), newVkImage, newMemory->VkDeviceMemory_(), newMemory->Offset());

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = newVkImage;
	viewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	VkImageView newImageView = VK_NULL_HANDLE;
	Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &viewInfo, nullptr, &newImageView));

	Graphic::Instance::Image* newImage = new Graphic::Instance::Image();
	newImage->_vkImageType = VkImageType::VK_IMAGE_TYPE_2D;
	newImage->_extent = { extent.width, extent.height, 1 };
	newImage->_vkFormat = format;
	newImage->_vkImageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	newImage->_vkImageUsage = static_cast<VkImageUsageFlagBits>(imageUsage);
	newImage->_mipLevels = 1;
	newImage->_vkSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	newImage->_vkMemoryProperty = static_cast<VkMemoryPropertyFlagBits>(memoryProperty);
	newImage->_vkImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
	newImage->_vkImageAspect = aspect;
	newImage->_vkImage = newVkImage;
	newImage->_vkImageView = newImageView;
	newImage->_memory = newMemory;
	newImage->_layerCount = 6;
	newImage->_perLayerSize = extent.width * extent.height * 4;

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::Create2DImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	imageInfo.extent = { extent.width, extent.height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = imageUsage;
	imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = 0;

	VkImage newVkImage = VK_NULL_HANDLE;
	Utils::Log::Exception("Failed to create image.", vkCreateImage(CoreObject::Instance::VkDevice_(), &imageInfo, nullptr, &newVkImage));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(CoreObject::Instance::VkDevice_(), newVkImage, &memRequirements);

	auto newMemory = new Instance::Memory(CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, memoryProperty));
	vkBindImageMemory(CoreObject::Instance::VkDevice_(), newVkImage, newMemory->VkDeviceMemory_(), newMemory->Offset());

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = newVkImage;
	viewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView newImageView = VK_NULL_HANDLE;
	Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &viewInfo, nullptr, &newImageView));

	Graphic::Instance::Image* newImage = new Graphic::Instance::Image();
	newImage->_vkImageType = VkImageType::VK_IMAGE_TYPE_2D;
	newImage->_extent = { extent.width, extent.height, 1 };
	newImage->_vkFormat = format;
	newImage->_vkImageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	newImage->_vkImageUsage = imageUsage;
	newImage->_mipLevels = 1;
	newImage->_vkSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	newImage->_vkMemoryProperty = memoryProperty;
	newImage->_vkImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	newImage->_vkImageAspect = aspect;
	newImage->_vkImage = newVkImage;
	newImage->_vkImageView = newImageView;
	newImage->_memory = newMemory;
	newImage->_layerCount = 1;
	newImage->_perLayerSize = extent.width * extent.height * 4;

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::CreateNative2DImage(VkImage vkImage, VkImageView vkImageView, VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkImageAspectFlags aspect)
{
	VkImage newVkImage = vkImage;

	auto newMemory = nullptr;

	VkImageView newImageView = vkImageView;

	Graphic::Instance::Image* newImage = new Graphic::Instance::Image();
	newImage->_isNative = true;
	newImage->_vkImageType = VkImageType::VK_IMAGE_TYPE_2D;
	newImage->_extent = { extent.width, extent.height, 1 };
	newImage->_vkFormat = format;
	newImage->_vkImageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	newImage->_vkImageUsage = imageUsage;
	newImage->_mipLevels = 1;
	newImage->_vkSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	newImage->_vkMemoryProperty = 0;
	newImage->_vkImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	newImage->_vkImageAspect = aspect;
	newImage->_vkImage = newVkImage;
	newImage->_vkImageView = newImageView;
	newImage->_memory = newMemory;
	newImage->_layerCount = 1;
	newImage->_perLayerSize = extent.width * extent.height * 4;

	return newImage;
}
