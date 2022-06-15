#include "Core/Graphic/Instance/Memory.h"
AirEngine::Core::Graphic::Instance::Memory::Memory(uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize start, VkDeviceSize size, std::mutex* mutex, VkMemoryPropertyFlags properties)
	: _memoryTypeIndex(memoryTypeIndex)
	, _vkMemory(memory)
	, _offset(start)
	, _size(size)
	, _mutex(mutex)
	, _isExclusive(false)
	, _properties(properties)
{

}
AirEngine::Core::Graphic::Instance::Memory::Memory(bool isExclusive, uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize start, VkDeviceSize size, std::mutex* mutex, VkMemoryPropertyFlags properties)
	: _memoryTypeIndex(memoryTypeIndex)
	, _vkMemory(memory)
	, _offset(start)
	, _size(size)
	, _mutex(mutex)
	, _isExclusive(isExclusive)
	, _properties(properties)
{

}

AirEngine::Core::Graphic::Instance::Memory::~Memory()
{
	//Controled by MemoryManager
}

std::mutex* AirEngine::Core::Graphic::Instance::Memory::Mutex()
{
	return _mutex;
}

VkDeviceSize AirEngine::Core::Graphic::Instance::Memory::Offset()
{
	return _offset;
}

VkDeviceSize AirEngine::Core::Graphic::Instance::Memory::Size()
{
	return _size;
}

VkDeviceMemory AirEngine::Core::Graphic::Instance::Memory::VkDeviceMemory_()
{
	return _vkMemory;
}

VkMemoryPropertyFlags AirEngine::Core::Graphic::Instance::Memory::VkMemoryPropertyFlags_()
{
	return _properties;
}
