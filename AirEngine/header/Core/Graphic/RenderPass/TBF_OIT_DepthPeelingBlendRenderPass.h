#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include <vector>

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
				class TBF_OIT_DepthPeelingBlendRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::Mesh* _fullScreenMesh;
					Material* _blendMaterial;
					Instance::ImageSampler* _colorTextureSampler;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
					void OnPrepare(Camera::CameraBase* camera)override;
				public:
					TBF_OIT_DepthPeelingBlendRenderPass();
					~TBF_OIT_DepthPeelingBlendRenderPass();
					TBF_OIT_DepthPeelingBlendRenderPass(const TBF_OIT_DepthPeelingBlendRenderPass&) = delete;
					TBF_OIT_DepthPeelingBlendRenderPass& operator=(const TBF_OIT_DepthPeelingBlendRenderPass&) = delete;
					TBF_OIT_DepthPeelingBlendRenderPass(TBF_OIT_DepthPeelingBlendRenderPass&&) = delete;
					TBF_OIT_DepthPeelingBlendRenderPass& operator=(TBF_OIT_DepthPeelingBlendRenderPass&&) = delete;

				};
			}
		}
	}
}