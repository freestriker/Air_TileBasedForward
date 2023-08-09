#pragma once
#include <vulkan/vulkan_core.h>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class ImageSampler
				{
				private:
					VkFilter _magFilter;
					VkFilter _minFilter;
					VkSamplerMipmapMode _mipmapMode;
					VkSamplerAddressMode _addressModeU;
					VkSamplerAddressMode _addressModeV;
					VkSamplerAddressMode _addressModeW;
					VkBool32 _anisotropyEnable;
					float _maxAnisotropy;
					VkBorderColor _borderColor;
					float _mipmapLevel;

					VkSampler _vkSampler;

					ImageSampler(const ImageSampler&) = delete;
					ImageSampler& operator=(const ImageSampler&) = delete;
					ImageSampler(ImageSampler&&) = delete;
					ImageSampler& operator=(ImageSampler&&) = delete;
				public:
					ImageSampler(
						VkFilter magFilter,
						VkFilter minFilter,
						VkSamplerMipmapMode mipmapMode,
						VkSamplerAddressMode addressModeU,
						VkSamplerAddressMode addressModeV,
						VkSamplerAddressMode addressModeW,
						float maxAnisotropy,
						VkBorderColor borderColor,
						float mipmapLevel = 0.0
					);
					ImageSampler(VkFilter filter);
					ImageSampler(
						VkFilter filter,
						VkSamplerMipmapMode mipmapMode,
						VkSamplerAddressMode addressMode,
						float maxAnisotropy,
						VkBorderColor borderColor,
						float mipmapLevel = 0.0
					);
					~ImageSampler();
					VkSampler VkSampler_();
					inline float MipmapLevel()const
					{
						return _mipmapLevel;
					}
				};
			}
		}
	}
}
