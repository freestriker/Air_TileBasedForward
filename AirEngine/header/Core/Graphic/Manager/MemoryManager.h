#pragma once
#include <vulkan/vulkan_core.h>
#include <map>
#include <vector>
#include <mutex>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Thread;
				class Instance;
			}
			namespace Instance
			{
				class Memory;
			}
			namespace Manager
			{
				class MemoryManager
				{
					friend class CoreObject::Instance;
				private:
					struct MemoryChunkUsage
					{
						VkDeviceSize offset;
						VkDeviceSize size;
						MemoryChunkUsage(VkDeviceSize offset, VkDeviceSize size);
						MemoryChunkUsage();
					};
					struct MemoryChunk
					{
						VkDeviceMemory memory;
						VkDeviceSize size;
						std::mutex mutex;
						std::map<VkDeviceSize, MemoryChunkUsage> allocated;
						std::map<VkDeviceSize, MemoryChunkUsage> unallocated;
						MemoryChunk(uint32_t typeIndex, VkDeviceSize size);
						~MemoryChunk();
					};
					struct MemoryChunkPool
					{
						std::map<VkDeviceMemory, MemoryChunk*> chunks;
						VkMemoryPropertyFlags properties;
						std::mutex mutex;
						uint32_t memoryTypeIndex;
						MemoryChunkPool();
					};
				private:
					std::vector< MemoryChunkPool*> _pools;
					VkDeviceSize const _defaultSize;
					MemoryManager(VkDeviceSize defaultSize);
					~MemoryManager();
				public:
					Instance::Memory AcquireMemory(VkMemoryRequirements& requirement, VkMemoryPropertyFlags properties);
					Instance::Memory AcquireExclusiveMemory(VkMemoryRequirements& requirement, VkMemoryPropertyFlags properties);
					void ReleaseMemory(Instance::Memory& memoryBlock);
					void Collect();
				};
			}
		}
	}
}
