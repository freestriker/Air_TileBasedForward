#include "Core/Graphic/Manager/MemoryManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Instance/Memory.h"

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryChunkUsage::MemoryChunkUsage(VkDeviceSize offset, VkDeviceSize size)
	: offset(offset)
	, size(size)
{
}

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryChunkUsage::MemoryChunkUsage()
	: offset(-1)
	, size(-1)
{
}

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryChunk::MemoryChunk(uint32_t typeIndex, VkDeviceSize size)
	: size(size)
	, mutex()
	, allocated({})
	, unallocated({ {0, Graphic::Manager::MemoryManager::MemoryChunkUsage(0, size)} })
	, memory(VK_NULL_HANDLE)

{
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = typeIndex;
	Utils::Log::Exception("Failed to allocate memory chunk.", vkAllocateMemory(Graphic::CoreObject::Instance::VulkanDevice_(), &allocInfo, nullptr, &memory));
}

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryChunk::~MemoryChunk()
{
	vkFreeMemory(Graphic::CoreObject::Instance::VulkanDevice_(), memory, nullptr);
}

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryManager(VkDeviceSize defaultSize)
	: _defaultSize(defaultSize)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(Graphic::CoreObject::Instance::VulkanPhysicalDevice_(), &memProperties);

	_pools.resize(memProperties.memoryTypeCount);
	for (size_t i = 0; i < _pools.size(); i++)
	{
		_pools[i] = new MemoryChunkPool();
		_pools[i]->memoryTypeIndex = static_cast<uint32_t>(i);
		_pools[i]->properties = memProperties.memoryTypes[i].propertyFlags;
	}
}

AirEngine::Core::Graphic::Manager::MemoryManager::~MemoryManager()
{
}

AirEngine::Core::Graphic::Instance::Memory AirEngine::Core::Graphic::Manager::MemoryManager::AcquireMemory(VkMemoryRequirements& requirement, VkMemoryPropertyFlags properties)
{
	VkDeviceSize newSize = (requirement.size + requirement.alignment - 1) & ~(requirement.alignment - 1);
	if (newSize > _defaultSize)
	{
		for (auto& pool : _pools)
		{
			if ((requirement.memoryTypeBits & (1 << pool->memoryTypeIndex)) && (_pools[pool->memoryTypeIndex]->properties & properties) == properties)
			{
				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = newSize;
				allocInfo.memoryTypeIndex = pool->memoryTypeIndex;

				VkDeviceMemory newMemory = VK_NULL_HANDLE;
				Utils::Log::Exception("Failed to allocate exculsive memory.", vkAllocateMemory(Graphic::CoreObject::Instance::VulkanDevice_(), &allocInfo, nullptr, &newMemory));
				std::mutex* newMutex = new std::mutex();

				return Instance::Memory(true, pool->memoryTypeIndex, newMemory, 0, newSize, newMutex, properties);
			}
		}
	}
	else
	{
		for (auto& pool : _pools)
		{
			if ((requirement.memoryTypeBits & (1 << pool->memoryTypeIndex)) && (_pools[pool->memoryTypeIndex]->properties & properties) == properties)
			{
				std::unique_lock<std::mutex> poolLocker(pool->mutex);

				for (auto& chunkPair : pool->chunks)
				{
					std::unique_lock<std::mutex> chunkLocker(chunkPair.second->mutex);

					for (auto& usagePair : chunkPair.second->unallocated)
					{
						if (usagePair.second.size < requirement.size) continue;

						VkDeviceSize newStart = (usagePair.second.offset + requirement.alignment - 1) & ~(requirement.alignment - 1);
						VkDeviceSize newEnd = newStart + newSize;

						VkDeviceSize oldStart = usagePair.second.offset;
						VkDeviceSize oldSize = usagePair.second.size;
						VkDeviceSize oldEnd = oldStart + oldSize;

						if (newEnd <= oldEnd)
						{
							if (newStart == oldStart)
							{
								chunkPair.second->unallocated.erase(oldStart);
							}
							else
							{
								usagePair.second.size = newStart - oldStart;
							}
							if (oldEnd > newEnd)
							{
								chunkPair.second->unallocated.emplace(newEnd, MemoryChunkUsage(newEnd, oldEnd - newEnd));
							}
							chunkPair.second->allocated.emplace(newStart, MemoryChunkUsage(newStart, newSize));
							return AirEngine::Core::Graphic::Instance::Memory(pool->memoryTypeIndex, chunkPair.second->memory, newStart, newSize, &chunkPair.second->mutex, properties);
						}
					}
				}

				MemoryChunk* newChunk = new MemoryChunk(pool->memoryTypeIndex, _defaultSize);
				newChunk->unallocated.clear();
				newChunk->unallocated.emplace(newSize, MemoryChunkUsage(newSize, _defaultSize - newSize));
				newChunk->allocated.emplace(0, MemoryChunkUsage(0, newSize));

				pool->chunks.emplace(newChunk->memory, newChunk);

				return Instance::Memory(pool->memoryTypeIndex, newChunk->memory, 0, newSize, &newChunk->mutex, properties);
			}
		}
	}

	Utils::Log::Exception("Failed to allocate memory.");
}

AirEngine::Core::Graphic::Instance::Memory AirEngine::Core::Graphic::Manager::MemoryManager::AcquireExclusiveMemory(VkMemoryRequirements& requirement, VkMemoryPropertyFlags properties)
{
	VkDeviceSize newSize = (requirement.size + requirement.alignment - 1) & ~(requirement.alignment - 1);
	for (auto& pool : _pools)
	{
		if ((requirement.memoryTypeBits & (1 << pool->memoryTypeIndex)) && (_pools[pool->memoryTypeIndex]->properties & properties) == properties)
		{
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = newSize;
			allocInfo.memoryTypeIndex = pool->memoryTypeIndex;

			VkDeviceMemory newMemory = VK_NULL_HANDLE;
			Utils::Log::Exception("Failed to allocate exculsive memory.", vkAllocateMemory(Graphic::CoreObject::Instance::VulkanDevice_(), &allocInfo, nullptr, &newMemory));
			std::mutex* newMutex = new std::mutex();

			return Instance::Memory(true, pool->memoryTypeIndex, newMemory, 0, newSize, newMutex, properties);
		}
	}

	Utils::Log::Exception("Failed to allocate exculsive memory.");
}

void AirEngine::Core::Graphic::Manager::MemoryManager::ReleaseMemory(Instance::Memory& memory)
{
	if (memory._isExclusive)
	{
		vkFreeMemory(Graphic::CoreObject::Instance::VulkanDevice_(), memory._vkMemory, nullptr);
		delete memory._mutex;
		return;
	}
	else
	{
		MemoryChunk* chunk = _pools[memory._memoryTypeIndex]->chunks[memory._vkMemory];

		{
			std::unique_lock<std::mutex> chunkLock(chunk->mutex);
			chunk->allocated.erase(memory._offset);

			VkDeviceSize recycleStart = memory._offset;
			VkDeviceSize recycleSize = memory._size;
			VkDeviceSize recycleEnd = recycleStart + recycleSize;

			bool merged = false;
			for (auto& usagePair : chunk->unallocated)
			{
				MemoryChunkUsage usage = usagePair.second;
				if (usage.offset < recycleStart)
				{
					if (usage.offset + usage.size == recycleStart)
					{
						merged = true;
						if (chunk->unallocated.count(recycleEnd))
						{
							usagePair.second.size = usage.size + recycleSize + chunk->unallocated[recycleEnd].size;
							chunk->unallocated.erase(recycleEnd);
						}
						else
						{
							usagePair.second.size = usage.size + recycleSize;
						}
						return;
					}
				}
				else
				{
					break;
				}
			}
			if (!merged)
			{
				chunk->unallocated.emplace(recycleStart, MemoryChunkUsage{ recycleStart , recycleSize });
				return;
			}
		}
		Utils::Log::Exception("Failed to release memory.");
	}
}

void AirEngine::Core::Graphic::Manager::MemoryManager::Collect()
{
	for (auto& pool : _pools)
	{
		std::unique_lock<std::mutex> poolLocker(pool->mutex);

		std::vector<VkDeviceMemory> emptyChunkKeys = std::vector<VkDeviceMemory>();
		for (auto& chunkPair : pool->chunks)
		{
			std::unique_lock<std::mutex> chunkLocker(chunkPair.second->mutex);

			if (chunkPair.second->allocated.size() == 0)
			{
				emptyChunkKeys.emplace_back(chunkPair.first);
			}
		}
		for (auto& emptyChunkKey : emptyChunkKeys)
		{
			delete pool->chunks[emptyChunkKey];
			pool->chunks.erase(emptyChunkKey);
		}
	}
}

AirEngine::Core::Graphic::Manager::MemoryManager::MemoryChunkPool::MemoryChunkPool()
	: chunks()
	, properties()
	, mutex()
	, memoryTypeIndex()
{
}
