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
				class OpaqueRenderPass final: public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Manager::RenderPassTarget* renderPassObject)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					OpaqueRenderPass();
					~OpaqueRenderPass();
					OpaqueRenderPass(const OpaqueRenderPass&) = delete;
					OpaqueRenderPass& operator=(const OpaqueRenderPass&) = delete;
					OpaqueRenderPass(OpaqueRenderPass&&) = delete;
					OpaqueRenderPass& operator=(OpaqueRenderPass&&) = delete;

				};
			}
		}
	}
}