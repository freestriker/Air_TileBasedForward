#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Buffer;
			}
			namespace Command
			{
				class BufferMemoryBarrier
				{
					VkBufferMemoryBarrier _vkBufferMemoryBarrier;
				public:
					BufferMemoryBarrier(Instance::Buffer* buffer, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags);
					BufferMemoryBarrier();
					const VkBufferMemoryBarrier& BufferMemoryBarrier_();
				};
			}
		}
	}
}
