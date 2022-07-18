#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class Buffer;
			}
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace RenderPass
			{
				class PreZRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Instance::Image* _depthImage;
					Instance::Buffer* _depthBuffer;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					PreZRenderPass();
					~PreZRenderPass();
					PreZRenderPass(const PreZRenderPass&) = delete;
					PreZRenderPass& operator=(const PreZRenderPass&) = delete;
					PreZRenderPass(PreZRenderPass&&) = delete;
					PreZRenderPass& operator=(PreZRenderPass&&) = delete;

					Instance::Image* DepthImage();
					Instance::Buffer* DepthBuffer();
				};
			}
		}
	}
}