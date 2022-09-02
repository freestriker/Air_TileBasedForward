#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <set>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				enum class ShaderSlotType;
			}
			namespace Instance
			{
				class DescriptorSet;
			}
			namespace CoreObject
			{
				class Instance;
				class Thread;
			}
			namespace Manager
			{
				class DescriptorSetManager
				{
					friend class CoreObject::Instance;
					friend class CoreObject::Thread;
				private:
					struct DescriptorSetChunkPool
					{
						Rendering::ShaderSlotType const slotType;
						uint32_t const chunkSize;
						std::vector<VkDescriptorPoolSize> const vkDescriptorPoolSizes;
						VkDescriptorPoolCreateInfo const chunkCreateInfo;

						std::mutex mutex;

						std::map<VkDescriptorPool, uint32_t> chunks;

						std::set<Instance::DescriptorSet*> handles;

						DescriptorSetChunkPool(Rendering::ShaderSlotType slotType, std::vector< VkDescriptorType>& types, uint32_t chunkSize);
						~DescriptorSetChunkPool();
						Instance::DescriptorSet* AcquireDescripterSet(VkDescriptorSetLayout descriptorSetLayout);
						void ReleaseDescripterSet(Instance::DescriptorSet*& descriptorSet);
						void Collect();

						static std::vector<VkDescriptorPoolSize> PopulateVkDescriptorPoolSizes(std::vector< VkDescriptorType>& types, int chunkSize);
						static VkDescriptorPoolCreateInfo PopulateChunkCreateInfo(uint32_t chunkSize, std::vector<VkDescriptorPoolSize> const& chunkSizes);
					};
					std::shared_mutex _managerMutex;
					std::map< Rendering::ShaderSlotType, DescriptorSetChunkPool*> _pools;

					DescriptorSetManager();
					~DescriptorSetManager();
					DescriptorSetManager(const DescriptorSetManager&) = delete;
					DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
					DescriptorSetManager(DescriptorSetManager&&) = delete;
					DescriptorSetManager& operator=(DescriptorSetManager&&) = delete;
				public:
					void AddDescriptorSetPool(Rendering::ShaderSlotType slotType, std::vector< VkDescriptorType> descriptorTypes, uint32_t chunkSize);
					void DeleteDescriptorSetPool(Rendering::ShaderSlotType slotType);
					Instance::DescriptorSet* AcquireDescripterSet(Rendering::ShaderSlotType slotType, VkDescriptorSetLayout descriptorSetLayout);
					void ReleaseDescripterSet(Instance::DescriptorSet*& descriptorSet);
					void Collect();

				};

			}

		}
	}
}