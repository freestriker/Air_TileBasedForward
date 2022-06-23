#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			enum class ShaderSlotType;
			namespace Manager
			{
				class DescriptorSetManager;
			}
			namespace Instance
			{
				class DescriptorSet
				{
					friend class Manager::DescriptorSetManager;
				public:
					struct DescriptorSetWriteData
					{
						VkDescriptorType type;
						VkBuffer buffer;
						VkDeviceSize offset;
						VkDeviceSize range;
						VkSampler sampler;
						VkImageView imageView;
						VkImageLayout imageLayout;
						DescriptorSetWriteData(VkDescriptorType type, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
						DescriptorSetWriteData(VkDescriptorType type, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);
					};
					VkDescriptorSet VkDescriptorSet_();
					void UpdateBindingData(std::vector<uint32_t> bindingIndex, std::vector<Graphic::Instance::DescriptorSet::DescriptorSetWriteData> data);
				private:
					ShaderSlotType _slotType;
					VkDescriptorSet _vkDescriptorSet;
					VkDescriptorPool _sourceVkDescriptorChunk;
					DescriptorSet(ShaderSlotType slotType, VkDescriptorSet descriptorSet, VkDescriptorPool sourceDescriptorChunk);
					~DescriptorSet();
					DescriptorSet(const DescriptorSet&) = delete;
					DescriptorSet& operator=(const DescriptorSet&) = delete;
					DescriptorSet(DescriptorSet&&) = delete;
					DescriptorSet& operator=(DescriptorSet&&) = delete;
				};
			}
		}
	}
}
