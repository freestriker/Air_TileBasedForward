#pragma once
#include <vulkan/vulkan_core.h>
#include <mutex>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Manager
			{
				class MemoryManager;
			}
			namespace Instance
			{
				class Memory final
				{
					friend class AirEngine::Core::Graphic::Manager::MemoryManager;
				private:
					bool _isExclusive;
					uint32_t _memoryTypeIndex;
					VkMemoryPropertyFlags _properties;
					VkDeviceMemory _vkMemory;
					VkDeviceSize _offset;
					VkDeviceSize _size;
					std::mutex* _mutex;

					Memory(uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize start, VkDeviceSize size, std::mutex* mutex, VkMemoryPropertyFlags property);
					Memory(bool isExclusive, uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize start, VkDeviceSize size, std::mutex* mutex, VkMemoryPropertyFlags property);
				public:
					~Memory();
					std::mutex* Mutex();
					VkDeviceSize Offset();
					VkDeviceSize Size();
					VkDeviceMemory VkDeviceMemory_();
					VkMemoryPropertyFlags VkMemoryPropertyFlags_();
				};

			}
		}
	}
}
