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
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = static_cast<VkDeviceSize>(size);
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	AirEngine::Utils::Log::Exception("Failed to create buffer.", vkCreateBuffer(Graphic::CoreObject::Instance::VulkanDevice_(), &bufferInfo, nullptr, &_vkBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Graphic::CoreObject::Instance::VulkanDevice_(), _vkBuffer, &memRequirements);

	_memory = new Graphic::Instance::Memory(Graphic::CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, properties));

	{
		std::unique_lock<std::mutex> lock(*_memory->Mutex());
		vkBindBufferMemory(Graphic::CoreObject::Instance::VulkanDevice_(), _vkBuffer, _memory->VkDeviceMemory_(), _memory->Offset());
	}
}

void AirEngine::Core::Graphic::Instance::Buffer::WriteData(const void* data, size_t dataSize)
{
	std::unique_lock<std::mutex> lock(*_memory->Mutex());
	{
		void* transferData;
		vkMapMemory(Graphic::CoreObject::Instance::VulkanDevice_(), _memory->VkDeviceMemory_(), _memory->Offset(), _memory->Size(), 0, &transferData);
		memcpy(transferData, data, dataSize);
		vkUnmapMemory(Graphic::CoreObject::Instance::VulkanDevice_(), _memory->VkDeviceMemory_());
	}
}

void AirEngine::Core::Graphic::Instance::Buffer::WriteData(std::function<void(void*)> writeFunction)
{
	std::unique_lock<std::mutex> lock(*_memory->Mutex());
	{
		void* transferData;
		vkMapMemory(Graphic::CoreObject::Instance::VulkanDevice_(), _memory->VkDeviceMemory_(), _memory->Offset(), _memory->Size(), 0, &transferData);
		writeFunction(transferData);
		vkUnmapMemory(Graphic::CoreObject::Instance::VulkanDevice_(), _memory->VkDeviceMemory_());
	}
}

VkBuffer AirEngine::Core::Graphic::Instance::Buffer::VkBuffer_()
{
	return VkBuffer();
}

AirEngine::Core::Graphic::Instance::Memory* AirEngine::Core::Graphic::Instance::Buffer::Memory()
{
	return _memory;
}

size_t AirEngine::Core::Graphic::Instance::Buffer::Size()
{
	return size_t();
}

size_t AirEngine::Core::Graphic::Instance::Buffer::Offset()
{
	return size_t();
}

AirEngine::Core::Graphic::Instance::Buffer::~Buffer()
{
	vkDestroyBuffer(Graphic::CoreObject::Instance::VulkanDevice_(), _vkBuffer, nullptr);
	Graphic::CoreObject::Instance::MemoryManager().ReleaseMemory(*_memory);

	delete _memory;
}
