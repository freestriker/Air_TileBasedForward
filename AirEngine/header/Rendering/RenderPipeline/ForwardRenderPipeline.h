#pragma once
#include "Core/Graphic/Rendering/RenderPipelineBase.h"

namespace AirEngine
{
	namespace Rendering
	{
		namespace RenderPipeline
		{
			class ForwardRenderPipeline final : public Core::Graphic::Rendering::RenderPipelineBase
			{
			public:
				ForwardRenderPipeline();
				virtual ~ForwardRenderPipeline();
				ForwardRenderPipeline(const ForwardRenderPipeline&) = delete;
				ForwardRenderPipeline& operator=(const ForwardRenderPipeline&) = delete;
				ForwardRenderPipeline(ForwardRenderPipeline&&) = delete;
				ForwardRenderPipeline& operator=(ForwardRenderPipeline&&) = delete;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderPipelineBase)
			};
		}
	}
}