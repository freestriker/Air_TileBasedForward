#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Memory.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/MemoryManager.h"
#include "Utils/Log.h"

AirEngine::Core::Graphic::Instance::Buffer::Buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	: _vkBuffer(VK_NULL_HANDLE)
	, _memory(nullptr)
	, _size(size)
	, _usage(usage)
	, _vkBufferView(VK_NULL_HANDLE)
	, _format()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = static_cast<VkDeviceSize>(size);
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	AirEngine::Utils::Log::Exception("Failed to create buffer.", vkCreateBuffer(Graphic::CoreObject::Instance::VkDevice_(), &bufferInfo, nullptr, &_vkBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Graphic::CoreObject::Instance::VkDevice_(), _vkBuffer, &memRequirements);

	_memory = new Graphic::Instance::Memory(Graphic::CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, properties));

	{
		std::unique_lock<std::mutex> lock(*_memory->Mutex());
		vkBindBufferMemory(Graphic::CoreObject::Instance::VkDevice_(), _vkBuffer, _memory->VkDeviceMemory_(), _memory->Offset());
	}
}

AirEngine::Core::Graphic::Instance::Buffer::Buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkFormat format)
	: _vkBuffer(VK_NULL_HANDLE)
	, _memory(nullptr)
	, _size(size)
	, _usage(usage)
	, _vkBufferView(VK_NULL_HANDLE)
	, _format(format)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = static_cast<VkDeviceSize>(size);
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	AirEngine::Utils::Log::Exception("Failed to create buffer.", vkCreateBuffer(Graphic::CoreObject::Instance::VkDevice_(), &bufferInfo, nullptr, &_vkBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Graphic::CoreObject::Instance::VkDevice_(), _vkBuffer, &memRequirements);

	_memory = new Graphic::Instance::Memory(Graphic::CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, properties));

	{
		std::unique_lock<std::mutex> lock(*_memory->Mutex());
		vkBindBufferMemory(Graphic::CoreObject::Instance::VkDevice_(), _vkBuffer, _memory->VkDeviceMemory_(), _memory->Offset());
	}

	VkBufferViewCreateInfo viewInfo{};
	viewInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	viewInfo.pNext = nullptr;
	viewInfo.flags = 0;
	viewInfo.buffer = _vkBuffer;
	viewInfo.format = format;
	viewInfo.offset = 0;
	viewInfo.range = _size;

	AirEngine::Utils::Log::Exception("Failed to create buffer view.", vkCreateBufferView(Graphic::CoreObject::Instance::VkDevice_(), &viewInfo, nullptr, &_vkBufferView));
}

void AirEngine::Core::Graphic::Instance::Buffer::WriteData(const void* data, size_t dataSize)
{
	std::unique_lock<std::mutex> lock(*_memory->Mutex());
	{
		void* transferData;
		vkMapMemory(Graphic::CoreObject::Instance::VkDevice_(), _memory->VkDeviceMemory_(), _memory->Offset(), _memory->Size(), 0, &transferData);
		memcpy(transferData, data, dataSize);
		vkUnmapMemory(Graphic::CoreObject::Instance::VkDevice_(), _memory->VkDeviceMemory_());
	}
}

void AirEngine::Core::Graphic::Instance::Buffer::WriteData(std::function<void(void*)> writeFunction)
{
	std::unique_lock<std::mutex> lock(*_memory->Mutex());
	{
		void* transferData;
		vkMapMemory(Graphic::CoreObject::Instance::VkDevice_(), _memory->VkDeviceMemory_(), _memory->Offset(), _memory->Size(), 0, &transferData);
		writeFunction(transferData);
		vkUnmapMemory(Graphic::CoreObject::Instance::VkDevice_(), _memory->VkDeviceMemory_());
	}
}

VkBuffer AirEngine::Core::Graphic::Instance::Buffer::VkBuffer_()
{
	return _vkBuffer;
}

VkBufferView AirEngine::Core::Graphic::Instance::Buffer::VkBufferView_()
{
	return _vkBufferView;
}

VkFormat AirEngine::Core::Graphic::Instance::Buffer::BufferViewFormat()
{
	return _format;
}

AirEngine::Core::Graphic::Instance::Memory* AirEngine::Core::Graphic::Instance::Buffer::Memory()
{
	return _memory;
}

size_t AirEngine::Core::Graphic::Instance::Buffer::Size()
{
	return _size;
}

size_t AirEngine::Core::Graphic::Instance::Buffer::Offset()
{
	return 0;
}

AirEngine::Core::Graphic::Instance::Buffer::~Buffer()
{
	vkDestroyBuffer(Graphic::CoreObject::Instance::VkDevice_(), _vkBuffer, nullptr);
	Graphic::CoreObject::Instance::MemoryManager().ReleaseMemory(*_memory);

	delete _memory;
}
