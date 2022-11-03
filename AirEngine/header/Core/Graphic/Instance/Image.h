#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include "Core/IO/Asset/AssetBase.h"
#include <json.hpp>
#include <map>
#include <FreeImage/FreeImage.h>

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
						uint32_t baseLayer;
						uint32_t layerCount;
						uint32_t baseMipmapLevel;
						uint32_t mipmapLevelCount;
						NLOHMANN_DEFINE_TYPE_INTRUSIVE(
							ImageViewInfo,
							imageViewType,
							imageAspectFlags,
							baseLayer,
							layerCount,
							baseMipmapLevel,
							mipmapLevelCount
						)
					};
					struct ImageInfo
					{
						std::vector<std::vector<std::string>> mipmapLayerSourcePaths;
						VkFormat format;
						FREE_IMAGE_TYPE targetType;
						VkImageTiling imageTiling;
						VkImageUsageFlags imageUsageFlags;
						VkMemoryPropertyFlags memoryPropertyFlags;
						VkImageCreateFlags imageCreateFlags;
						bool autoGenerateMipmap;
						bool topDown;
						std::map<std::string, ImageViewInfo> imageViewInfos;

						NLOHMANN_DEFINE_TYPE_INTRUSIVE(
							ImageInfo,
							mipmapLayerSourcePaths,
							format,
							targetType,
							imageTiling,
							imageUsageFlags,
							memoryPropertyFlags,
							imageCreateFlags,
							autoGenerateMipmap,
							topDown,
							imageViewInfos
						)
					};
					struct ImageView
					{
						friend class Image;
					private:
						VkImageView vkImageView;
						VkImageSubresourceRange vkImageSubresourceRange;
						std::vector<VkExtent2D>* vkExtent2Ds;
					public:
						VkImageView VkImageView_();
						const VkImageSubresourceRange& VkImageSubresourceRange_();
						uint32_t BaseMipmapLevel();
						uint32_t MipmapLevelCount();
						uint32_t BaseLayer();
						uint32_t LayerCount();
						VkImageAspectFlags VkImageAspectFlags_();
						VkExtent2D VkExtent2D_(uint32_t levelIndex);
						VkExtent3D VkExtent3D_(uint32_t levelIndex);
					};

				private:
					bool _isNative;
					ImageInfo _imageImfo;
					VkImage _vkImage;
					Memory* _memory;
					VkExtent2D _vkExtent2D;
					std::vector<VkExtent2D> _vkExtent2Ds;
					std::map<std::string, ImageView> _imageViews;
					uint32_t _layerCount;
					uint32_t _mipmapLevelCount;

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

					void AddImageView(std::string name, VkImageViewType imageViewType, VkImageAspectFlags imageAspectFlags, uint32_t baseArrayLayer, uint32_t layerCount, uint32_t baseMipmapLevel = 0, uint32_t mipmapLevelCount = 1);
					void RemoveImageView(std::string name);

					ImageView& ImageView_(std::string imageViewName = "DefaultImageView");
					VkImage VkImage_();
					VkExtent3D VkExtent3D_();
					VkExtent2D VkExtent2D_();
					uint32_t LayerCount();
					VkFormat VkFormat_();
					VkImageUsageFlags VkImageUsageFlags_();
					VkMemoryPropertyFlags VkMemoryPropertyFlags_();
					VkImageTiling VkImageTiling_();
					Memory& Memory_();

				private:
					void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
					void CreateVulkanInstance();
				};

			}
		}
	}
}
