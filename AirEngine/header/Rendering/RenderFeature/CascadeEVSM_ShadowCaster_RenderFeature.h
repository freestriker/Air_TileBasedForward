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
			class CascadeEVSM_ShadowCaster_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class CascadeEVSM_ShadowCaster_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(CascadeEVSM_ShadowCaster_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};
				class CascadeEVSM_Blit_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(CascadeEVSM_Blit_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};
				class CascadeEVSM_Blur_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(CascadeEVSM_Blur_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class CascadeEVSM_ShadowCaster_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class CascadeEVSM_ShadowCaster_RenderFeature;
				private:
					Core::Graphic::Rendering::RenderPassBase* shadowCasterRenderPass;
					Core::Graphic::Instance::ImageSampler* pointSampler;

					std::array<Core::Graphic::Instance::Image*, CASCADE_COUNT> depthAttachemnts;
					std::array<Core::Graphic::Rendering::FrameBuffer*, CASCADE_COUNT> shadowCasterFrameBuffers;
					Core::Graphic::Instance::Buffer* lightCameraInfoBuffer;
					Core::Graphic::Instance::Buffer* lightCameraInfoStagingBuffer;
					Core::Graphic::Instance::Buffer* csmShadowReceiverInfoBuffer;
				public:
					std::array<float, CASCADE_COUNT> frustumSegmentScales;
					std::array<float, CASCADE_COUNT> lightCameraCompensationDistances;
					std::array<uint32_t, CASCADE_COUNT> shadowImageResolutions;
					float overlapScale;

					void Refresh();

					void SetShadowReceiverMaterialParameters(Core::Graphic::Rendering::Material* material);

					CONSTRUCTOR(CascadeEVSM_ShadowCaster_RenderFeatureData)
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
					alignas(16) glm::mat4 matrixVC2PL[CASCADE_COUNT];
					alignas(16) glm::vec4 texelSize[CASCADE_COUNT];
				};

				CONSTRUCTOR(CascadeEVSM_ShadowCaster_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _shadowCasterRenderPass;
				std::string _shadowCasterRenderPassName;
				Core::Graphic::Instance::ImageSampler* _pointSampler;

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