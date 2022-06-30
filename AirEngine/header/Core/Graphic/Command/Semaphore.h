#pragma once
#include <vulkan/vulkan_core.h>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class Semaphore
				{
				private:
					VkSemaphore _vkSemaphore;
					Semaphore(const Semaphore&) = delete;
					Semaphore& operator=(const Semaphore&) = delete;
					Semaphore(Semaphore&&) = delete;
					Semaphore& operator=(Semaphore&&) = delete;
				public:
					VkSemaphore VkSemphore_();
					Semaphore();
					~Semaphore();
				};
			}
		}
	}
}
