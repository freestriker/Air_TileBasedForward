#pragma once
#include <map>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include "Core/Graphic/Instance/DescriptorSet.h"
#include "Core/Logic/Object/Object.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Manager
			{
				class DescriptorSetManager;
			}
			namespace Instance
			{
				class Buffer;
				class Image;
				class ImageSampler;
				class DescriptorSet;
			}

			namespace Rendering
			{
				enum class ShaderSlotType;
				class Shader;
				class Material final : Core::Logic::Object::Object
				{
				private:
					struct Slot
					{
						std::string name;
						uint32_t setIndex;
						Core::Logic::Object::Object* asset;
						ShaderSlotType slotType;
						Instance::DescriptorSet* descriptorSet;
					};

				private:
					Shader* _shader;
					std::map<std::string, Slot> _slots;
					void OnDestroy()override;
				public:
					Material(Shader* shader);
					~Material();
					Material(const Material&) = delete;
					Material& operator=(const Material&) = delete;
					Material(Material&&) = delete;
					Material& operator=(Material&&) = delete;

					void SetSlotData(std::string name, std::vector<uint32_t> bindingIndex, std::vector< Graphic::Instance::DescriptorSet::DescriptorSetWriteData> data);
					AirEngine::Core::Graphic::Instance::Image* GetSampledImageCube(std::string name);
					void SetSampledImageCube(std::string slotName, AirEngine::Core::Graphic::Instance::Image* imageCube, AirEngine::Core::Graphic::Instance::ImageSampler* sampler, std::string imageViewName = "DefaultImageView");
					AirEngine::Core::Graphic::Instance::Image* GetSampledImage2D(std::string name);
					void SetSampledImage2D(std::string slotName, AirEngine::Core::Graphic::Instance::Image* image2D, AirEngine::Core::Graphic::Instance::ImageSampler* sampler, std::string imageViewName = "DefaultImageView");
					AirEngine::Core::Graphic::Instance::Image* GetStorageImage2D(std::string name);
					void SetStorageImage2D(std::string slotName, AirEngine::Core::Graphic::Instance::Image* image2D, std::string imageViewName = "DefaultImageView");
					AirEngine::Core::Graphic::Instance::Buffer* GetUniformBuffer(std::string name);
					void SetUniformBuffer(std::string name, Instance::Buffer* buffer);
					Instance::Buffer* GetStorageBuffer(std::string name);
					void SetStorageBuffer(std::string name, Instance::Buffer* buffer);
					Instance::Buffer* GetUniformTexelBuffer(std::string name);
					void SetUniformTexelBuffer(std::string name, Instance::Buffer* buffer);
					Instance::Buffer* GetStorgeTexelBuffer(std::string name);
					void SetStorgeTexelBuffer(std::string name, Instance::Buffer* buffer);
					VkPipelineLayout PipelineLayout();
					std::vector<VkDescriptorSet> VkDescriptorSets();
					Shader* Shader();

				};
			}
		}
	}
}
