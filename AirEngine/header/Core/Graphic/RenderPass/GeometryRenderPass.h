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
				class GeometryRenderPass final : public RenderPassBase
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
					GeometryRenderPass();
					~GeometryRenderPass();
					GeometryRenderPass(const GeometryRenderPass&) = delete;
					GeometryRenderPass& operator=(const GeometryRenderPass&) = delete;
					GeometryRenderPass(GeometryRenderPass&&) = delete;
					GeometryRenderPass& operator=(GeometryRenderPass&&) = delete;

					Instance::Image* DepthImage();
					Instance::Buffer* DepthBuffer();
				};
			}
		}
	}
}