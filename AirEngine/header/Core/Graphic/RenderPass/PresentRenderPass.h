#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"

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
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			class Material;
			namespace Instance
			{
				class ImageSampler;
			}
			namespace RenderPass
			{
				class PresentRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::Mesh* _fullScreenMesh;
					Material* _material;
					Instance::ImageSampler* _imageSampler;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					PresentRenderPass();
					~PresentRenderPass();
					PresentRenderPass(const PresentRenderPass&) = delete;
					PresentRenderPass& operator=(const PresentRenderPass&) = delete;
					PresentRenderPass(PresentRenderPass&&) = delete;
					PresentRenderPass& operator=(PresentRenderPass&&) = delete;

				};
			}
		}
	}
}