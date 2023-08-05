#pragma once
#include "Core/Graphic/Rendering/RenderPipelineBase.h"

namespace AirEngine
{
	namespace Rendering
	{
		namespace RenderPipeline
		{
			class BaseRenderPipeline final : public Core::Graphic::Rendering::RenderPipelineBase
			{
			public:
				BaseRenderPipeline();
				virtual ~BaseRenderPipeline();
				BaseRenderPipeline(const BaseRenderPipeline&) = delete;
				BaseRenderPipeline& operator=(const BaseRenderPipeline&) = delete;
				BaseRenderPipeline(BaseRenderPipeline&&) = delete;
				BaseRenderPipeline& operator=(BaseRenderPipeline&&) = delete;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderPipelineBase)
			};
		}
	}
}