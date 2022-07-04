#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace Instance
			{
				class Buffer;
			}
			namespace RenderPass
			{
				class TBFOpaqueRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Instance::Buffer* _depthStorageBuffer;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Manager::RenderPassTarget* renderPassObject)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TBFOpaqueRenderPass();
					~TBFOpaqueRenderPass();
					TBFOpaqueRenderPass(const TBFOpaqueRenderPass&) = delete;
					TBFOpaqueRenderPass& operator=(const TBFOpaqueRenderPass&) = delete;
					TBFOpaqueRenderPass(TBFOpaqueRenderPass&&) = delete;
					TBFOpaqueRenderPass& operator=(TBFOpaqueRenderPass&&) = delete;

				};
			}
		}
	}
}