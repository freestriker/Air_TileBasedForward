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
				class Fence
				{
				private:
					VkFence _vkFence;
					Fence(const Fence&) = delete;
					Fence& operator=(const Fence&) = delete;
					Fence(Fence&&) = delete;
					Fence& operator=(Fence&&) = delete;
				public:
					Fence();
					Fence(VkFenceCreateFlags flag);
					~Fence();
					VkFence VkFence_();
					void Reset();
					void Wait();
				};
			}
		}
	}
}