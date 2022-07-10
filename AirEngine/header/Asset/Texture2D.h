#pragma once
#include "Core/IO/Asset/AssetBase.h"
#include <glm/glm.hpp>
#include <vector>
#include <rttr/type>
#include <vulkan/vulkan_core.h>
#include <json.hpp>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class Buffer;
				class ImageSampler;
			}
			namespace Command
			{
				class CommandBuffer;
			}
		}
	}
	namespace Asset
	{
		class Texture2D final : public Core::IO::Asset::AssetBase
		{
		public:
			struct TextureInfo
			{
				alignas(16) glm::vec4 size;
				alignas(16) glm::vec4 tilingScale;
			};
			struct Texture2DSettings
			{
				std::string imagePath;
				VkSampleCountFlags sampleCount;
				VkFormat format;
				VkImageUsageFlags usage;
				bool useSampler;
				VkFilter magFilter;
				VkFilter minFilter;
				VkSamplerAddressMode addressMode;
				float anisotropy;
				VkBorderColor borderColor;
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(
					Texture2DSettings,
					imagePath,
					sampleCount,
					format,
					usage,
					useSampler,
					magFilter,
					minFilter,
					addressMode,
					anisotropy,
					borderColor
				);
			};
		private:
			VkExtent2D _extent;
			Core::Graphic::Instance::Buffer* _textureInfoBuffer;
			Core::Graphic::Instance::Image* _image;
			Core::Graphic::Instance::ImageSampler* _imageSampler;
			Texture2D::TextureInfo _textureInfo;
			Texture2DSettings _settings;
			void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
		public:
			Texture2D();
			Texture2D(VkExtent2D extent, Texture2DSettings settings);
			~Texture2D();
			void WriteData(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, std::vector<unsigned char>& byteData);
			VkExtent2D VkExtent2D_();
			Core::Graphic::Instance::Buffer* InfoBuffer();
			Core::Graphic::Instance::Image* Image();
			Core::Graphic::Instance::ImageSampler* ImageSampler();
			Texture2D::TextureInfo Info();
			Texture2DSettings Settings();

			RTTR_ENABLE(AirEngine::Core::IO::Asset::AssetBase)
		};
	}
}