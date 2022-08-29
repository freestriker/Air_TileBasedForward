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
			class GeometryRenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class GeometryRenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					GeometryRenderPass();
					virtual ~GeometryRenderPass();
					GeometryRenderPass(const GeometryRenderPass&) = delete;
					GeometryRenderPass& operator=(const GeometryRenderPass&) = delete;
					GeometryRenderPass(GeometryRenderPass&&) = delete;
					GeometryRenderPass& operator=(GeometryRenderPass&&) = delete;
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GeometryRenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
				public:
					GeometryRenderFeatureData();
					virtual ~GeometryRenderFeatureData();
					GeometryRenderFeatureData(const GeometryRenderFeatureData&) = delete;
					GeometryRenderFeatureData& operator=(const GeometryRenderFeatureData&) = delete;
					GeometryRenderFeatureData(GeometryRenderFeatureData&&) = delete;
					GeometryRenderFeatureData& operator=(GeometryRenderFeatureData&&) = delete;
				public:
					Core::Graphic::Rendering::FrameBuffer* frameBuffer;
					Core::Graphic::Instance::Image* depthTexture;
					Core::Graphic::Instance::Image* normalTexture;

					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				GeometryRenderFeature();
				virtual ~GeometryRenderFeature();
				GeometryRenderFeature(const GeometryRenderFeature&) = delete;
				GeometryRenderFeature& operator=(const GeometryRenderFeature&) = delete;
				GeometryRenderFeature(GeometryRenderFeature&&) = delete;
				GeometryRenderFeature& operator=(GeometryRenderFeature&&) = delete;

			private:
				Core::Graphic::Rendering::RenderPassBase* _geometryRenderPass;
				std::string _geometryRenderPassName;

				Core::Graphic::Rendering::RenderFeatureDataBase* OnCreateRenderFeatureData(Camera::CameraBase* camera)override;
				void OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)override;
				void OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				void OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;
				void OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*>const* rendererComponents)override;
				void OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;
				void OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureBase)
			};
		}
	}
}