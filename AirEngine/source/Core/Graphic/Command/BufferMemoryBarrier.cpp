#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/Buffer.h"

AirEngine::Core::Graphic::Command::BufferMemoryBarrier::BufferMemoryBarrier(Instance::Buffer* buffer, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags)
	: _vkBufferMemoryBarrier()
{
	_vkBufferMemoryBarrier.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	_vkBufferMemoryBarrier.pNext = nullptr;
	_vkBufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_vkBufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_vkBufferMemoryBarrier.srcAccessMask = srcAccessFlags;
	_vkBufferMemoryBarrier.dstAccessMask = dstAccessFlags;
	_vkBufferMemoryBarrier.buffer = buffer->VkBuffer_();
	_vkBufferMemoryBarrier.offset = buffer->Offset();
	_vkBufferMemoryBarrier.size = buffer->Size();
}

AirEngine::Core::Graphic::Command::BufferMemoryBarrier::BufferMemoryBarrier()
	: _vkBufferMemoryBarrier()
{
}

const VkBufferMemoryBarrier& AirEngine::Core::Graphic::Command::BufferMemoryBarrier::BufferMemoryBarrier_()
{
	return _vkBufferMemoryBarrier;
}
