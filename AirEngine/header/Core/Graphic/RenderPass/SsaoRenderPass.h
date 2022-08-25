#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"
#define NOISE_IMAGE_WIDTH 64
#define SAMPLE_KERNAL_SIZE 64
#define SAMPLE_BIAS_ANGLE 20
#define SAMPLE_KERNAL_RADIUS 0.6f
#define BLUR_SAMPLE_OFFSET_FACTOR 1.7f

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
				class SsaoBlurRenderPass final : public RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					SsaoBlurRenderPass();
					~SsaoBlurRenderPass();
					SsaoBlurRenderPass(const SsaoBlurRenderPass&) = delete;
					SsaoBlurRenderPass& operator=(const SsaoBlurRenderPass&) = delete;
					SsaoBlurRenderPass(SsaoBlurRenderPass&&) = delete;
					SsaoBlurRenderPass& operator=(SsaoBlurRenderPass&&) = delete;
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
					struct BlurInfo
					{
						alignas(8)	glm::vec2 attachmentSize;
						alignas(8)	glm::vec2 attachmentTexelSize;
						alignas(8)	glm::vec2 sampleOffset;
					};
					struct SampleKernal
					{
						alignas(4) float radius;
						alignas(16) glm::vec4 points[SAMPLE_KERNAL_SIZE];
					};
					RenderPass::RenderPassBase* _occlusionRenderPass;
					RenderPass::RenderPassBase* _blurRenderPass;
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Asset::Mesh* _fullScreenMesh;
					Material* _occlusionMaterial;
					Material* _blendMaterial;
					Material* _horizontalBlurMaterial;
					Material* _verticalBlurMaterial;
					Instance::Image* _occlusionImage;
					Instance::Image* _temporaryOcclusionImage;
					Instance::Image* _noiseImage;
					Instance::ImageSampler* _noiseTextureSampler;
					Instance::ImageSampler* _normalTextureSampler;
					Instance::ImageSampler* _depthTextureSampler;
					Instance::ImageSampler* _occlusionTextureSampler;
					Manager::RenderPassTarget* _occlusionRenderPassTarget;
					Manager::RenderPassTarget* _horizontalBlurRenderPassTarget;
					Manager::RenderPassTarget* _verticalBlurRenderPassTarget;
					Instance::Buffer* _sampleKernalBuffer;
					Instance::Buffer* _sizeInfoBuffer;
					Instance::Buffer* _horizontalBlurInfoBuffer;
					Instance::Buffer* _verticalBlurInfoBuffer;
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