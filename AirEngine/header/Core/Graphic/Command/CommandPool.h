#pragma once
#include <vulkan/vulkan_core.h>
#include <map>
#include <string>
#include <memory>
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandBuffer;
				class CommandPool final
				{
					friend class CommandBuffer;
				private:
					VkCommandPool  _vkCommandPool;
					std::map<std::string, CommandBuffer*> _commandBuffers;
					std::string _queueName;
				public:
					CommandPool(std::string queueName, VkCommandPoolCreateFlags flag);
					~CommandPool();

					CommandPool(const CommandPool&) = delete;
					CommandPool& operator=(const CommandPool&) = delete;
					CommandPool(CommandPool&&) = delete;
					CommandPool& operator=(CommandPool&&) = delete;

					VkCommandPool VkCommandPool_();


					CommandBuffer* CreateCommandBuffer(std::string name, VkCommandBufferLevel level);
					CommandBuffer* GetCommandBuffer(std::string name);
					void DestoryCommandBuffer(std::string name);
					void DestoryCommandBuffer(CommandBuffer* commandBuffer);
					void Reset();
				};
			}
		}
	}
}
