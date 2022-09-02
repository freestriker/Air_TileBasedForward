#include "Core/Graphic/Manager/DescriptorSetManager.h"
#include "Core/Graphic/Instance/DescriptorSet.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::AddDescriptorSetPool(Rendering::ShaderSlotType slotType, std::vector<VkDescriptorType> descriptorTypes, uint32_t chunkSize)
{
	std::unique_lock<std::shared_mutex> lock(_managerMutex);

	_pools[slotType] = new DescriptorSetChunkPool(slotType, descriptorTypes, chunkSize);
}

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::DeleteDescriptorSetPool(Rendering::ShaderSlotType slotType)
{
	std::unique_lock<std::shared_mutex> lock(_managerMutex);

	delete _pools[slotType];
	_pools.erase(slotType);
}

AirEngine::Core::Graphic::Instance::DescriptorSet* AirEngine::Core::Graphic::Manager::DescriptorSetManager::AcquireDescripterSet(Rendering::ShaderSlotType slotType, VkDescriptorSetLayout descriptorSetLayout)
{
	std::shared_lock<std::shared_mutex> lock(_managerMutex);

	return _pools[slotType]->AcquireDescripterSet(descriptorSetLayout);
}

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::ReleaseDescripterSet(Instance::DescriptorSet*& descriptorSet)
{
	std::shared_lock<std::shared_mutex> lock(_managerMutex);

	_pools[descriptorSet->_slotType]->ReleaseDescripterSet(descriptorSet);
}

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::Collect()
{
	std::unique_lock<std::shared_mutex> lock(_managerMutex);

	for (auto& poolPair : _pools)
	{
		poolPair.second->Collect();
	}
}

AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetManager()
	: _managerMutex()
	, _pools()
{
}

AirEngine::Core::Graphic::Manager::DescriptorSetManager::~DescriptorSetManager()
{
	std::unique_lock<std::shared_mutex> lock(_managerMutex);
	for (auto& poolPair : _pools)
	{
		delete poolPair.second;
	}
}

AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::DescriptorSetChunkPool(Rendering::ShaderSlotType slotType, std::vector<VkDescriptorType>& types, uint32_t chunkSize)
	: slotType(slotType)
	, mutex()
	, chunkSize(chunkSize)
	, vkDescriptorPoolSizes(PopulateVkDescriptorPoolSizes(types, chunkSize))
	, chunkCreateInfo(PopulateChunkCreateInfo(chunkSize, vkDescriptorPoolSizes))
	, chunks()
	, handles()
{
}

AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::~DescriptorSetChunkPool()
{
	std::unique_lock<std::mutex> lock(mutex);

	for (const auto& handle : handles)
	{
		vkFreeDescriptorSets(CoreObject::Instance::VkDevice_(), handle->_sourceVkDescriptorChunk, 1, &handle->_vkDescriptorSet);

		delete handle;
	}

	for (const auto& chunkPair : chunks)
	{
		vkResetDescriptorPool(CoreObject::Instance::VkDevice_(), chunkPair.first, 0);
		vkDestroyDescriptorPool(CoreObject::Instance::VkDevice_(), chunkPair.first, nullptr);
	}
}

AirEngine::Core::Graphic::Instance::DescriptorSet* AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::AcquireDescripterSet(VkDescriptorSetLayout descriptorSetLayout)
{
	std::unique_lock<std::mutex> lock(mutex);

	VkDescriptorPool useableChunk = VK_NULL_HANDLE;
	for (const auto& chunkPair : chunks)
	{
		if (chunkPair.second > 0)
		{
			useableChunk = chunkPair.first;
			break;
		}
	}
	if (useableChunk == VK_NULL_HANDLE)
	{
		Utils::Log::Exception("Failed to create descriptor chunk.", vkCreateDescriptorPool(CoreObject::Instance::VkDevice_(), &chunkCreateInfo, nullptr, &useableChunk));
		chunks.emplace(useableChunk, chunkSize);
	}
	--chunks[useableChunk];

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = useableChunk;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	VkDescriptorSet newVkSet = VK_NULL_HANDLE;
	Utils::Log::Exception("Failed to allocate descriptor sets.", vkAllocateDescriptorSets(CoreObject::Instance::VkDevice_(), &allocInfo, &newVkSet));

	Instance::DescriptorSet* newSet = new Instance::DescriptorSet(slotType, newVkSet, useableChunk);

	handles.emplace(newSet);

	return newSet;
}

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::ReleaseDescripterSet(Instance::DescriptorSet*& descriptorSet)
{
	std::unique_lock<std::mutex> lock(mutex);

	++chunks[descriptorSet->_sourceVkDescriptorChunk];
	vkFreeDescriptorSets(CoreObject::Instance::VkDevice_(), descriptorSet->_sourceVkDescriptorChunk, 1, &descriptorSet->_vkDescriptorSet);

	handles.erase(descriptorSet);

	delete descriptorSet;
	descriptorSet = nullptr;
}

void AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::Collect()
{
	std::unique_lock<std::mutex> lock(mutex);

	for (auto it = chunks.begin(); it != chunks.end(); )
	{
		if (it->second == chunkSize)
		{
			vkResetDescriptorPool(CoreObject::Instance::VkDevice_(), it->first, 0);
			vkDestroyDescriptorPool(CoreObject::Instance::VkDevice_(), it->first, nullptr);
			it = chunks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

std::vector<VkDescriptorPoolSize> AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::PopulateVkDescriptorPoolSizes(std::vector<VkDescriptorType>& types, int chunkSize)
{
	std::map<VkDescriptorType, uint32_t> typeCounts = std::map<VkDescriptorType, uint32_t>();
	for (const auto& descriptorType : types)
	{
		if (!typeCounts.count(descriptorType))
		{
			typeCounts[descriptorType] = 0;
		}
		++typeCounts[descriptorType];
	}

	std::vector<VkDescriptorPoolSize> poolSizes = std::vector<VkDescriptorPoolSize>(typeCounts.size());
	size_t poolSizeIndex = 0;
	for (const auto& pair : typeCounts)
	{
		poolSizes[poolSizeIndex].type = pair.first;
		poolSizes[poolSizeIndex].descriptorCount = pair.second * chunkSize;

		poolSizeIndex++;
	}

	return poolSizes;
}

VkDescriptorPoolCreateInfo AirEngine::Core::Graphic::Manager::DescriptorSetManager::DescriptorSetChunkPool::PopulateChunkCreateInfo(uint32_t chunkSize, std::vector<VkDescriptorPoolSize> const& chunkSizes)
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(chunkSizes.size());
	poolInfo.pPoolSizes = chunkSizes.data();
	poolInfo.maxSets = chunkSize;
	return poolInfo;
}
