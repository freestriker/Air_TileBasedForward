#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>

#define NOISE_COUNT 4096
#define SAMPLE_POINT_COUNT 32

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
			namespace Rendering
			{
				class Shader;
				class Material;
			}
			namespace Instance
			{
				class ImageSampler;
			}
		}
	}
	namespace Rendering
	{
		namespace RenderFeature
		{
			class SSAO_Occlusion_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class SSAO_Occlusion_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(SSAO_Occlusion_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class SSAO_Occlusion_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class SSAO_Occlusion_RenderFeature;
				private:
					Core::Graphic::Rendering::Material* material;
					Core::Graphic::Rendering::FrameBuffer* frameBuffer;
					Core::Graphic::Instance::Buffer* ssaoInfoBuffer;
					Core::Graphic::Instance::Buffer* samplePointInfoBuffer;
					Core::Graphic::Instance::Image* noiseTexture;
					Core::Graphic::Instance::Buffer* noiseStagingBuffer;
				public:
					float samplePointRadius;
					float samplePointBiasAngle;
					int noiseTextureWidth;
					Core::Graphic::Instance::Image* occlusionTexture;
					Core::Graphic::Instance::Image* depthTexture;
					Core::Graphic::Instance::Image* normalTexture;

					CONSTRUCTOR(SSAO_Occlusion_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(SSAO_Occlusion_RenderFeature)

			private:
				struct SsaoInfo
				{
					alignas(8) glm::vec2 attachmentSize;
					alignas(8) glm::vec2 attachmentTexelSize;
					alignas(4) int noiseTextureWidth;
				};
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				Asset::Mesh* _fullScreenMesh;
				Core::Graphic::Rendering::Shader* _ssaoShader;
				Core::Graphic::Instance::ImageSampler* _textureSampler;

				Core::Graphic::Rendering::RenderFeatureDataBase* OnCreateRenderFeatureData(Camera::CameraBase* camera)override;
				void OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)override;
				void OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				void OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;
				void OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*>const* rendererComponents)override;
				void OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;
				void OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureBase)
			};
		}
	}
}