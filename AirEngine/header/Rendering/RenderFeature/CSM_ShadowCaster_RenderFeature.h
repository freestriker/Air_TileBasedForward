#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <array>
#include <glm/glm.hpp>

#define CASCADE_COUNT 4

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
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
			class CSM_ShadowCaster_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class CSM_ShadowCaster_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(CSM_ShadowCaster_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class CSM_ShadowCaster_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class CSM_ShadowCaster_RenderFeature;
				private:
					Core::Graphic::Rendering::RenderPassBase* shadowMapRenderPass;
					Core::Graphic::Instance::ImageSampler* sampler;

					Core::Graphic::Instance::Image* shadowImageArray;
					std::array<Core::Graphic::Rendering::FrameBuffer*, CASCADE_COUNT> shadowFrameBuffers;
					Core::Graphic::Instance::Buffer* lightCameraInfoBuffer;
					Core::Graphic::Instance::Buffer* lightCameraInfoHostStagingBuffer;
					Core::Graphic::Instance::Buffer* csmShadowReceiverInfoBuffer;
				public:
					std::array<float, CASCADE_COUNT> frustumSegmentScales;
					std::array<float, CASCADE_COUNT> lightCameraCompensationDistances;
					uint32_t shadowImageResolutions;
					std::array <glm::vec2, CASCADE_COUNT> bias;
					float overlapScale;
					int sampleHalfWidth;

					void Refresh();

					void SetShadowReceiverMaterialParameters(Core::Graphic::Rendering::Material* material);

					CONSTRUCTOR(CSM_ShadowCaster_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				struct LightCameraInfo
				{
					glm::mat4 view;
					glm::mat4 projection;
					glm::mat4 viewProjection;
				};
				struct CsmShadowReceiverInfo
				{
					alignas(16) glm::vec4 thresholdVZ[CASCADE_COUNT * 2];
					alignas(16) glm::vec3 wLightDirection;
					alignas(16) glm::vec3 vLightDirection;
					alignas(16) glm::vec4 bias[CASCADE_COUNT];
					alignas(16) glm::mat4 matrixVC2PL[CASCADE_COUNT];
					alignas(16) glm::vec4 texelSize[CASCADE_COUNT];
					alignas(4) int sampleHalfWidth;
				};

				CONSTRUCTOR(CSM_ShadowCaster_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _shadowMapRenderPass;
				std::string _shadowMapRenderPassName;
				Core::Graphic::Instance::ImageSampler* shadowImageSampler;

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