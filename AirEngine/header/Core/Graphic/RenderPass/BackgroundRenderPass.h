#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class ImageSampler;
			}
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace RenderPass
			{
				class BackgroundRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Instance::Image* _temporaryDepthImage;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					BackgroundRenderPass();
					~BackgroundRenderPass();
					BackgroundRenderPass(const BackgroundRenderPass&) = delete;
					BackgroundRenderPass& operator=(const BackgroundRenderPass&) = delete;
					BackgroundRenderPass(BackgroundRenderPass&&) = delete;
					BackgroundRenderPass& operator=(BackgroundRenderPass&&) = delete;

				};
			}
		}
	}
}