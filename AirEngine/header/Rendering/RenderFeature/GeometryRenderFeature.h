#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"

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

					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				GeometryRenderFeature();
				virtual ~GeometryRenderFeature();
				GeometryRenderFeature(const GeometryRenderFeature&) = delete;
				GeometryRenderFeature& operator=(const GeometryRenderFeature&) = delete;
				GeometryRenderFeature(GeometryRenderFeature&&) = delete;
				GeometryRenderFeature& operator=(GeometryRenderFeature&&) = delete;

				virtual Core::Graphic::Rendering::RenderFeatureDataBase* OnCreateRenderFeatureData()override;
				virtual void OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureBase)
			};
		}
	}
}