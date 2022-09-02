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
			class AO_Blur_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class AO_Blur_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(AO_Blur_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class AO_Blur_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class AO_Blur_RenderFeature;
				private:
					Core::Graphic::Rendering::Material* horizontalMaterial;
					Core::Graphic::Rendering::Material* verticalMaterial;
					Core::Graphic::Rendering::FrameBuffer* horizontalFrameBuffer;
					Core::Graphic::Rendering::FrameBuffer* verticalFrameBuffer;
					Core::Graphic::Instance::Buffer* horizontalBlurInfoBuffer;
					Core::Graphic::Instance::Buffer* verticalBlurInfoBuffer;
					Core::Graphic::Instance::Image* temporaryOcclusionTexture;
				public:
					float sampleOffsetFactor;
					int iterateCount;
					Core::Graphic::Instance::Image* occlusionTexture;
					Core::Graphic::Instance::Image* normalTexture;

					CONSTRUCTOR(AO_Blur_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(AO_Blur_RenderFeature)

			private:
				struct BlurInfo
				{
					alignas(8) glm::vec2 size;
					alignas(8) glm::vec2 texelSize;
					alignas(8) glm::vec2 sampleOffset;
				};
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				Asset::Mesh* _fullScreenMesh;
				Core::Graphic::Rendering::Shader* _blurShader;
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