#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"

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
				class F_OpaqueRenderPass final: public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::TextureCube* _ambientLightTexture;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					F_OpaqueRenderPass();
					~F_OpaqueRenderPass();
					F_OpaqueRenderPass(const F_OpaqueRenderPass&) = delete;
					F_OpaqueRenderPass& operator=(const F_OpaqueRenderPass&) = delete;
					F_OpaqueRenderPass(F_OpaqueRenderPass&&) = delete;
					F_OpaqueRenderPass& operator=(F_OpaqueRenderPass&&) = delete;

				};
			}
		}
	}
}