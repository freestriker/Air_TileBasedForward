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
				class NewImage: public Core::IO::Asset::AssetBase
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
						VkImageType imageType;
						VkFormat format;
						VkImageTiling imageTiling;
						VkImageUsageFlags imageUsageFlags;
						VkMemoryPropertyFlags memoryPropertyFlags;
						VkImageCreateFlags imageCreateFlags;
						std::map<std::string, ImageViewInfo> imageViewInfos;

						NLOHMANN_DEFINE_TYPE_INTRUSIVE(
							ImageInfo,
							subresourcePaths,
							imageType,
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

				public:
					NewImage();
					~NewImage();
					NewImage(const NewImage&) = delete;
					NewImage& operator=(const NewImage&) = delete;
					NewImage(NewImage&&) = delete;
					NewImage& operator=(NewImage&&) = delete;

					const ImageView& ImageView_(std::string imageViewName = "DefaultImageView");
					VkImage VkImage_();
					VkExtent3D VkExtent3D_();
					size_t PerLayerSize();
					uint32_t LayerCount();

				private:
					bool _isNative;
					ImageInfo _imageImfo;
					VkImage _vkImage;
					Memory* _memory;
					VkExtent2D _vkExtent2D;
					std::map<std::string, ImageView> _imageViews;
					size_t _perLayerSize;
				private:
					void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
				};

			}
		}
	}
}
