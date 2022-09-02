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
	namespace Asset
	{
		class Texture2D;
		class TextureCube;
	}
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

					void RefreshSlotData(std::vector<std::string> slotNames);
					void SetSlotData(std::string name, std::vector<uint32_t> bindingIndex, std::vector< Graphic::Instance::DescriptorSet::DescriptorSetWriteData> data);
					Asset::TextureCube* GetTextureCube(std::string name);
					void SetTextureCube(std::string name, Asset::TextureCube* textureCube);
					Asset::Texture2D* GetTexture2D(std::string name);
					void SetTexture2D(std::string name, Asset::Texture2D* texture2d);
					Asset::Texture2D* GetStorageTexture2D(std::string name);
					void SetStorgeTexture2D(std::string name, Asset::Texture2D* texture2d);
					Instance::Buffer* GetUniformBuffer(std::string name);
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
