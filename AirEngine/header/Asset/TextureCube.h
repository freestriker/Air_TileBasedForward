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
		class TextureCube final : public Core::IO::Asset::AssetBase
		{
		public:
			struct TextureCubeSetting
			{
				std::array<std::string, 6> paths;
				VkSamplerMipmapMode mipmapMode;
				VkSampleCountFlagBits sampleCount;
				VkFormat format;
				VkFilter filter;
				VkSamplerAddressMode addressMode;
				float anisotropy;
				VkBorderColor borderColor;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(
					TextureCubeSetting,
					paths,
					mipmapMode,
					sampleCount,
					format,
					filter,
					addressMode,
					anisotropy,
					borderColor
				);
			};
		private:
			VkExtent2D _extent;
			Core::Graphic::Instance::Image* _image;
			Core::Graphic::Instance::ImageSampler* _imageSampler;
			std::array<std::vector<unsigned char>, 6> _byteDatas;
			TextureCubeSetting _settings;
			void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
		public:
			TextureCube();
			TextureCube(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, VkExtent2D extent, TextureCubeSetting settings, std::array<std::vector<unsigned char>, 6>& byteDatas);
			~TextureCube();
			VkExtent2D VkExtent2D_();
			Core::Graphic::Instance::Image* Image();
			Core::Graphic::Instance::ImageSampler* ImageSampler();
			std::array<std::vector<unsigned char>, 6>* ByteData();
			TextureCubeSetting Settings();

			RTTR_ENABLE(AirEngine::Core::IO::Asset::AssetBase)
		};
	}
}