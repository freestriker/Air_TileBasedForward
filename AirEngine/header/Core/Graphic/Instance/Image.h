#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include "Core/IO/Asset/AssetBase.h"
#include <json.hpp>
#include <map>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Memory;
				class Image: public Core::IO::Asset::AssetBase
				{
				public:
					struct ImageViewInfo
					{
						VkImageViewType imageViewType;
						VkImageAspectFlags imageAspectFlags;
						uint32_t baseArrayLayer;
						uint32_t layerCount;
						NLOHMANN_DEFINE_TYPE_INTRUSIVE(
							ImageViewInfo,
							imageViewType,
							imageAspectFlags,
							baseArrayLayer,
							layerCount
						)
					};
					struct ImageInfo
					{
						std::vector<std::string> subresourcePaths;
						VkFormat format;
						VkImageTiling imageTiling;
						VkImageUsageFlags imageUsageFlags;
						VkMemoryPropertyFlags memoryPropertyFlags;
						VkImageCreateFlags imageCreateFlags;
						std::map<std::string, ImageViewInfo> imageViewInfos;

						NLOHMANN_DEFINE_TYPE_INTRUSIVE(
							ImageInfo,
							subresourcePaths,
							format,
							imageTiling,
							imageUsageFlags,
							memoryPropertyFlags,
							imageCreateFlags,
							imageViewInfos
						)
					};
					struct ImageView
					{
						VkImageView vkImageView;
						VkImageSubresourceRange vkImageSubresourceRange;
						VkImageSubresourceLayers vkImageSubresourceLayers;
					};

				private:
					bool _isNative;
					ImageInfo _imageImfo;
					VkImage _vkImage;
					Memory* _memory;
					VkExtent2D _vkExtent2D;
					std::map<std::string, ImageView> _imageViews;
					uint32_t _layerCount;

				public:
					static Image* Create2DImage(
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkMemoryPropertyFlags memoryProperty,
						VkImageAspectFlags aspect,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL
					);
					static Image* CreateNative2DImage(
						VkImage vkImage,
						VkImageView vkImageView,
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkImageAspectFlags aspect
					);
					static Image* Create2DImageArray(
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkMemoryPropertyFlags memoryProperty,
						VkImageAspectFlags aspect,
						uint32_t arraySize,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL
					);
					static Image* CreateCubeImage(
						VkExtent2D extent,
						VkFormat format,
						VkImageUsageFlags imageUsage,
						VkMemoryPropertyFlags memoryProperty,
						VkImageAspectFlags aspect,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL
					);

					Image();
					~Image();
					Image(const Image&) = delete;
					Image& operator=(const Image&) = delete;
					Image(Image&&) = delete;
					Image& operator=(Image&&) = delete;

					void AddImageView(std::string name, VkImageViewType imageViewType, VkImageAspectFlags imageAspectFlags, uint32_t baseArrayLayer, uint32_t layerCount);
					void RemoveImageView(std::string name);

					const ImageView& ImageView_(std::string imageViewName = "DefaultImageView");
					VkImage VkImage_();
					VkExtent3D VkExtent3D_();
					VkExtent2D VkExtent2D_();
					uint32_t LayerCount();
					VkFormat VkFormat_();
					VkMemoryPropertyFlags VkMemoryPropertyFlags_();
					Memory& Memory_();

				private:
					void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
					void CreateVulkanInstance();
				};

			}
		}
	}
}
