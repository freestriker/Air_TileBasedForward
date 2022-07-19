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
				class Memory;
				class Image
				{
				public:
					~Image();
					VkImage VkImage_();
					VkImageView VkImageView_();
					VkExtent3D VkExtent3D_();
					Memory& Memory_();
					VkFormat VkFormat_();
					VkSampleCountFlags VkSampleCountFlags_();
					std::vector<VkImageSubresourceRange> VkImageSubresourceRanges_();
					std::vector<VkImageSubresourceLayers> VkImageSubresourceLayers_();
					uint32_t LayerCount();
					size_t PerLayerSize();
					VkImageUsageFlags VkImageUsageFlags_();
					VkMemoryPropertyFlags VkMemoryPropertyFlags_();
					VkImageAspectFlags VkImageAspectFlags_();

					static Image* CreateCubeImage(
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkMemoryPropertyFlags memoryProperty,
						VkImageAspectFlags aspect
					);
					static Image* Create2DImage(
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkMemoryPropertyFlags memoryProperty,
						VkImageAspectFlags aspect
					);
					static Image* CreateNative2DImage(
						VkImage vkImage,
						VkImageView vkImageView,
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkImageAspectFlags aspect
					);
				private:
					bool _isNative;

					VkImageType _vkImageType;
					VkExtent3D _extent;
					VkFormat _vkFormat;
					VkImageTiling _vkImageTiling;
					VkImageUsageFlags _vkImageUsage;
					uint32_t _mipLevels;
					VkSampleCountFlags _vkSampleCount;
					VkMemoryPropertyFlags _vkMemoryProperty;
					VkImageViewType _vkImageViewType;
					VkImageAspectFlags _vkImageAspect;
					uint32_t _layerCount;
					size_t _perLayerSize;

					VkImage _vkImage;
					VkImageView _vkImageView;
					Memory* _memory;

					Image();
					Image(const Image&) = delete;
					Image& operator=(const Image&) = delete;
					Image(Image&&) = delete;
					Image& operator=(Image&&) = delete;
				};

			}
		}
	}
}
