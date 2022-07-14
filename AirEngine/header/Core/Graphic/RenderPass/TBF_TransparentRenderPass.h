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
			namespace Instance
			{
				class Buffer;
			}
			namespace RenderPass
			{
				class TBF_TransparentRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::TextureCube* _ambientLightTexture;
					Instance::Buffer* _transparentLightListsBuffer;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TBF_TransparentRenderPass();
					~TBF_TransparentRenderPass();
					TBF_TransparentRenderPass(const TBF_TransparentRenderPass&) = delete;
					TBF_TransparentRenderPass& operator=(const TBF_TransparentRenderPass&) = delete;
					TBF_TransparentRenderPass(TBF_TransparentRenderPass&&) = delete;
					TBF_TransparentRenderPass& operator=(TBF_TransparentRenderPass&&) = delete;

				};
			}
		}
	}
}