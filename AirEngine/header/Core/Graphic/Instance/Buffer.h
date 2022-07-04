#pragma once
#include <vulkan/vulkan_core.h>
#include <functional>
#include "Core/Logic/Object/Object.h"

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
				class Buffer final: public Core::Logic::Object::Object
				{
				private:
					VkBuffer _vkBuffer;
					VkBufferView _vkBufferView;
					Memory* _memory;
					size_t _size;
					VkBufferUsageFlags _usage;
					VkFormat _format;

					Buffer(const Buffer& source) = delete;
					Buffer& operator=(const Buffer&) = delete;
					Buffer(Buffer&&) = delete;
					Buffer& operator=(Buffer&&) = delete;
				public:
					Buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
					Buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkFormat format);
					void WriteData(const void* data, size_t dataSize);
					void WriteData(std::function<void(void*)> writeFunction);
					VkBuffer VkBuffer_();
					VkBufferView VkBufferView_();
					VkFormat BufferViewFormat();
					Memory* Memory();
					size_t Size();
					size_t Offset();
					~Buffer();
				};
			}
		}
	}
}