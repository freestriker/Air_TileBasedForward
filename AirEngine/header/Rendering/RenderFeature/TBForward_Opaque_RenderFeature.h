#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"

namespace AirEngine
{
	namespace Rendering
	{
		namespace RenderFeature
		{
			class TBForward_Opaque_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class TBForward_Opaque_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(TBForward_Opaque_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class TBForward_Opaque_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class TBForward_Opaque_RenderFeature;
				private:
					Core::Graphic::Rendering::FrameBuffer* frameBuffer;
				public:
					bool needClearColorAttachment;
					Core::Graphic::Instance::Buffer* opaqueLightIndexListsBuffer;

					Core::Graphic::Rendering::RenderFeatureDataBase* csmShadowMapRenderFeatureData;

					CONSTRUCTOR(TBForward_Opaque_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(TBForward_Opaque_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				std::string _renderPassName;

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