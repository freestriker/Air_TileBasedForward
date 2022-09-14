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
	namespace Rendering
	{
		namespace RenderFeature
		{
			class CSM_ShadowMap_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class CSM_ShadowMap_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(CSM_ShadowMap_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class CSM_ShadowMap_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class CSM_ShadowMap_RenderFeature;
				private:
					Core::Graphic::Rendering::RenderPassBase* shadowMapRenderPass;

					std::array<Core::Graphic::Instance::Image*, CASCADE_COUNT> shadowImages;
					std::array<Core::Graphic::Rendering::FrameBuffer*, CASCADE_COUNT> shadowFrameBuffers;
					Core::Graphic::Instance::Buffer* lightCameraInfoBuffer;
					Core::Graphic::Instance::Buffer* lightCameraInfoStagingBuffer;
				public:
					std::array<float, CASCADE_COUNT> frustumSplitScales;
					std::array<float, CASCADE_COUNT> lightCameraCompensationDistances;
					std::array<uint32_t, CASCADE_COUNT> shadowImageResolutions;

					void Refresh();

					CONSTRUCTOR(CSM_ShadowMap_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				struct LightCameraInfo
				{
					glm::mat4 view;
					glm::mat4 projection;
					glm::mat4 viewProjection;
				};

				CONSTRUCTOR(CSM_ShadowMap_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _shadowMapRenderPass;
				std::string _shadowMapRenderPassName;

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