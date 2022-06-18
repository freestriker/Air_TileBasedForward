#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
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
				class Image;
			}
			namespace Command
			{
				class CommandPool;
				class Semaphore;
				class ImageMemoryBarrier;
				class CommandBuffer final
				{
					friend class CommandPool;
				private:
					struct CommandData
					{
						uint32_t indexCount;
					};
					CommandPool* _parentCommandPool;
					VkCommandBuffer _vkCommandBuffer;
					VkFence _vkFence;

					CommandData _commandData;
				public:
					std::string const _name;

				private:
					CommandBuffer(std::string name, CommandPool* parentCommandPool, VkCommandBufferLevel level);
					~CommandBuffer();

					CommandBuffer(const CommandBuffer&) = delete;
					CommandBuffer& operator=(const CommandBuffer&) = delete;
					CommandBuffer(CommandBuffer&&) = delete;
					CommandBuffer& operator=(CommandBuffer&&) = delete;
				public:
					void Reset();
					void BeginRecord(VkCommandBufferUsageFlags flag);
					void AddPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <ImageMemoryBarrier*> imageMemoryBarriers);
					void AddPipelineBarrier(VkDependencyFlags dependencyFlag, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers);
					void CopyBufferToImage(Instance::Buffer* srcBuffer, Instance::Image* dstImage, VkImageLayout dstImageLayout);
					void CopyBuffer(Instance::Buffer* srcBuffer, Instance::Buffer* dstBuffer);
					void CopyBuffer(Instance::Buffer* srcBuffer, VkDeviceSize srcOffset, Instance::Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size);
					void EndRecord();
					void Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages, std::vector<Command::Semaphore*> signalSemaphores);
					void Submit(std::vector<Command::Semaphore*> signalSemaphores);
					void Submit();
					void WaitForFinish();
				};
			}
		}
	}
}

