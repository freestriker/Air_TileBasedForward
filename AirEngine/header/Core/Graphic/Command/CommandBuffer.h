#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>

namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
	}
	namespace Core
	{
		namespace Graphic
		{
			class Shader;
			class Material;
			namespace RenderPass
			{
				class RenderPassBase;
			}
			namespace Instance
			{
				class Buffer;
				class Image;
			}
			namespace Manager
			{
				class RenderPassTarget;
			}
			namespace Command
			{
				class CommandPool;
				class Semaphore;
				class ImageMemoryBarrier;
				class BufferMemoryBarrier;
				class CommandBuffer final
				{
					friend class CommandPool;
				private:
					struct CommandData
					{
						uint32_t indexCount{};
						VkViewport viewport{};
						VkRect2D scissor{};
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
					void AddPipelineImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <ImageMemoryBarrier*> imageMemoryBarriers);
					void AddPipelineImageBarrier(VkDependencyFlags dependencyFlag, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers);
					void AddPipelineBufferBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <BufferMemoryBarrier*> bufferMemoryBarriers);
					void AddPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <ImageMemoryBarrier*> imageMemoryBarriers, std::vector <BufferMemoryBarrier*> bufferMemoryBarriers);
					void CopyBufferToImage(Instance::Buffer* srcBuffer, Instance::Image* dstImage, VkImageLayout dstImageLayout);
					void CopyImageToBuffer(Instance::Image* srcImage, Instance::Buffer* dstBuffer, VkImageLayout dstImageLayout);
					void CopyBuffer(Instance::Buffer* srcBuffer, Instance::Buffer* dstBuffer);
					void CopyBuffer(Instance::Buffer* srcBuffer, VkDeviceSize srcOffset, Instance::Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size);
					void EndRecord();
					void Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages, std::vector<Command::Semaphore*> signalSemaphores);
					void Submit(std::vector<Command::Semaphore*> signalSemaphores);
					void Submit();
					void WaitForFinish();
					void BeginRenderPass(Graphic::RenderPass::RenderPassBase* renderPass, Graphic::Manager::RenderPassTarget* renderPassObject, std::vector<VkClearValue> clearValues);
					void EndRenderPass();
					void BindMaterial(Material* material);
					void DrawMesh(Asset::Mesh* mesh);
					void Blit(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Image* dstImage, VkImageLayout dstImageLayout, VkFilter filter);
					void Blit(Instance::Image* srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, VkOffset3D offset1, VkOffset3D offset2, VkFilter filter);
				};
			}
		}
	}
}

