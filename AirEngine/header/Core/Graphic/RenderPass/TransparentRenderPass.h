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
			namespace RenderPass
			{
				class TransparentRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Manager::RenderPassTarget* renderPassObject)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TransparentRenderPass();
					~TransparentRenderPass();
					TransparentRenderPass(const TransparentRenderPass&) = delete;
					TransparentRenderPass& operator=(const TransparentRenderPass&) = delete;
					TransparentRenderPass(TransparentRenderPass&&) = delete;
					TransparentRenderPass& operator=(TransparentRenderPass&&) = delete;

				};
			}
		}
	}
}