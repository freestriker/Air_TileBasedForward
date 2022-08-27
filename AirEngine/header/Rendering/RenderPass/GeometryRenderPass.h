#pragma once
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Asset/TextureCube.h"

namespace AirEngine
{
	namespace Rendering
	{
		namespace RenderPass
		{
			class GeometryRenderPass final : public Core::Graphic::Rendering::RenderPassBase
			{
			private:
				void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
			public:
				GeometryRenderPass();
				~GeometryRenderPass();
				GeometryRenderPass(const GeometryRenderPass&) = delete;
				GeometryRenderPass& operator=(const GeometryRenderPass&) = delete;
				GeometryRenderPass(GeometryRenderPass&&) = delete;
				GeometryRenderPass& operator=(GeometryRenderPass&&) = delete;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
			};
		}
	}
}