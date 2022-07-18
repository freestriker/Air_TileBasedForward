#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"
#include <vector>
#include <array>
#define DEPTH_PEELING_STEP_COUNT 4

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
				class Image;
				class ImageSampler;
			}
			namespace Manager
			{
				class RenderPassTarget;
			}
			class Material;
			namespace RenderPass
			{
				class TBF_OIT_DepthPeelingRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Instance::Image* _thresholdDepthTexture;
					Instance::Image* _depthAttachment;
					std::array<Instance::Image*, DEPTH_PEELING_STEP_COUNT> _colorAttachments;
					std::array<Manager::RenderPassTarget*, DEPTH_PEELING_STEP_COUNT> _renderPassTargets;
					Instance::ImageSampler* _depthTextureSampler;

					bool _needDepthPeelingPass;

					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TBF_OIT_DepthPeelingRenderPass();
					~TBF_OIT_DepthPeelingRenderPass();
					TBF_OIT_DepthPeelingRenderPass(const TBF_OIT_DepthPeelingRenderPass&) = delete;
					TBF_OIT_DepthPeelingRenderPass& operator=(const TBF_OIT_DepthPeelingRenderPass&) = delete;
					TBF_OIT_DepthPeelingRenderPass(TBF_OIT_DepthPeelingRenderPass&&) = delete;
					TBF_OIT_DepthPeelingRenderPass& operator=(TBF_OIT_DepthPeelingRenderPass&&) = delete;

					std::array<Instance::Image*, DEPTH_PEELING_STEP_COUNT> PeeledColorImages();
					bool NeedDepthPeelingPass();

				};
			}
		}
	}
}