#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"
#define NOISE_IMAGE_WIDTH 64
#define SAMPLE_KERNAL_SIZE 64
#define SAMPLE_BIAS_ANGLE 35
#define SAMPLE_KERNAL_RADIUS 0.1f

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
			class Material;
			namespace Instance
			{
				class Image;
				class Buffer;
				class ImageSampler;
			}
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace Manager
			{
				class RenderPassTarget;
			}
			namespace RenderPass
			{
				class SsaoOcclusionRenderPass final : public RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					SsaoOcclusionRenderPass();
					~SsaoOcclusionRenderPass();
					SsaoOcclusionRenderPass(const SsaoOcclusionRenderPass&) = delete;
					SsaoOcclusionRenderPass& operator=(const SsaoOcclusionRenderPass&) = delete;
					SsaoOcclusionRenderPass(SsaoOcclusionRenderPass&&) = delete;
					SsaoOcclusionRenderPass& operator=(SsaoOcclusionRenderPass&&) = delete;
				};
				class SsaoRenderPass final : public RenderPassBase
				{
				private:
					struct SizeInfo
					{
						alignas(8)	glm::vec2 attachmentSize;
						alignas(8)	glm::vec2 noiseTextureSize;
						alignas(8)	glm::vec2 scale;
					};
					struct SampleKernal
					{
						alignas(4) float radius;
						alignas(16) glm::vec4 points[SAMPLE_KERNAL_SIZE];
					};
					RenderPass::RenderPassBase* _occlusionRenderPass;
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::Mesh* _fullScreenMesh;
					Material* _occlusionMaterial;
					Material* _blendMaterial;
					Instance::Image* _occlusionImage;
					Instance::Image* _colorImage;
					Instance::Image* _noiseImage;
					Instance::ImageSampler* _noiseTextureSampler;
					Instance::ImageSampler* _normalTextureSampler;
					Instance::ImageSampler* _depthTextureSampler;
					Manager::RenderPassTarget* _occlusionRenderPassTarget;
					Instance::Buffer* _sampleKernalBuffer;
					Instance::Buffer* _sizeInfoBuffer;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					SsaoRenderPass();
					~SsaoRenderPass();
					SsaoRenderPass(const SsaoRenderPass&) = delete;
					SsaoRenderPass& operator=(const SsaoRenderPass&) = delete;
					SsaoRenderPass(SsaoRenderPass&&) = delete;
					SsaoRenderPass& operator=(SsaoRenderPass&&) = delete;
				};
			}
		}
	}
}