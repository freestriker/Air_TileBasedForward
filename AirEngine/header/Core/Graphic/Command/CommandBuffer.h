#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
#include <map>
#include "Core/Graphic/Rendering/Material.h"

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
			namespace Instance
			{
				class Buffer;
				class Image;
			}
			namespace Rendering
			{
				class RenderPassBase;
				class FrameBuffer;
				class Material;
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
					template<typename TConstant>
					void PushConstant(Rendering::Material* material, VkShaderStageFlags stage, TConstant&& constant);
					void AddPipelineImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <ImageMemoryBarrier*> imageMemoryBarriers);
					void AddPipelineImageBarrier(VkDependencyFlags dependencyFlag, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector<ImageMemoryBarrier*> imageMemoryBarriers);
					void AddPipelineBufferBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <BufferMemoryBarrier*> bufferMemoryBarriers);
					void AddPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, std::vector <ImageMemoryBarrier*> imageMemoryBarriers, std::vector <BufferMemoryBarrier*> bufferMemoryBarriers);
					void CopyImage(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Image* dstImage, VkImageLayout dstImageLayout);
					void CopyImage(Instance::Image* srcImage, std::string srcImageViewName, VkImageLayout srcImageLayout, Instance::Image* dstImage, std::string dstImageViewName, VkImageLayout dstImageLayout);
					void CopyBufferToImage(Instance::Buffer* srcBuffer, Instance::Image* dstImage, std::string&& imageViewName, VkImageLayout dstImageLayout, uint32_t mipmapLevelOffset = 0);
					void CopyBufferToImage(Instance::Buffer* srcBuffer, Instance::Image* dstImage, VkImageLayout dstImageLayout, uint32_t mipmapLevelOffset = 0);
					void CopyBufferToImage(Instance::Buffer* srcBuffer, size_t srcBufferOffset, Instance::Image* dstImage, VkImageLayout dstImageLayout, uint32_t mipmapLevelOffset = 0);
					void CopyImageToBuffer(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Buffer* dstBuffer, uint32_t mipmapLevelOffset = 0);
					void FillBuffer(Instance::Buffer* dstBuffer, uint32_t data);
					void CopyBuffer(Instance::Buffer* srcBuffer, Instance::Buffer* dstBuffer);
					void CopyBuffer(Instance::Buffer* srcBuffer, VkDeviceSize srcOffset, Instance::Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size);
					void EndRecord();
					void ClearDepthImage(Instance::Image* image, VkImageLayout layout, float depth);
					void ClearColorImage(Instance::Image* image, VkImageLayout layout, VkClearColorValue targetColor);
					void Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages, std::vector<Command::Semaphore*> signalSemaphores);
					void Submit(std::vector<Command::Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages);
					void Submit(std::vector<Command::Semaphore*> signalSemaphores);
					void Submit();
					void WaitForFinish();
					void BeginRenderPass(Graphic::Rendering::RenderPassBase* renderPass, Graphic::Rendering::FrameBuffer* frameBuffer, std::map<std::string, VkClearValue> clearValues);
					void BeginRenderPass(Graphic::Rendering::RenderPassBase* renderPass, Graphic::Rendering::FrameBuffer* frameBuffer);
					void EndRenderPass();
					void DrawMesh(Asset::Mesh* mesh, Rendering::Material* material);
					void Dispatch(Rendering::Material* material, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
					void Blit(Instance::Image* srcImage, std::string&& srcImageViewName, VkImageLayout srcImageLayout, Instance::Image* dstImage, std::string&& dstImageViewName, VkImageLayout dstImageLayout, VkFilter filter);
					void Blit(Instance::Image* srcImage, VkImageLayout srcImageLayout, Instance::Image* dstImage, VkImageLayout dstImageLayout, VkFilter filter);
					
					CommandPool* ParentCommandPool();
				};
				template<typename TConstant>
				inline void CommandBuffer::PushConstant(Rendering::Material* material, VkShaderStageFlags stage, TConstant&& constant)
				{
					vkCmdPushConstants(_vkCommandBuffer, material->Shader()->VkPipelineLayout_(), stage, 0, sizeof(TConstant), &constant);
				}
			}
		}
	}
}

