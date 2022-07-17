#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"

namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
		class TextureCube;
	}
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
			class Material;
			namespace RenderPass
			{
				class BackgroundRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Material* _material;
					Asset::Mesh* _fullScreenMesh;
					Asset::TextureCube* _backgroundTexture;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
					void OnPrepare(Camera::CameraBase* camera)override;
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