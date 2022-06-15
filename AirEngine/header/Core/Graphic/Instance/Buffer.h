#pragma once
#include <vulkan/vulkan_core.h>
#include <functional>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			class CommandBuffer;
			namespace Instance
			{
				class Memory;
				class Buffer
				{
				private:
					VkBuffer _vkBuffer;
					Memory* _memory;
					size_t _size;
					VkBufferUsageFlags _usage;

					Buffer(const Buffer& source) = delete;
					Buffer& operator=(const Buffer&) = delete;
					Buffer(Buffer&&) = delete;
					Buffer& operator=(Buffer&&) = delete;
				public:
					Buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
					void WriteData(const void* data, size_t dataSize);
					void WriteData(std::function<void(void*)> writeFunction);
					VkBuffer VkBuffer_();
					Memory* Memory();
					size_t Size();
					size_t Offset();
					~Buffer();
				};
			}
		}
	}
}