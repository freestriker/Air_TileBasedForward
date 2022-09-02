#pragma once
#include "Core/Graphic/Rendering/RenderPassBase.h"

namespace AirEngine
{
	namespace Rendering
	{
		class PresentRenderPass final : public Core::Graphic::Rendering::RenderPassBase
		{
		private:
			void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
		public:
			PresentRenderPass();
			virtual ~PresentRenderPass();
			PresentRenderPass(const PresentRenderPass&) = delete;
			PresentRenderPass& operator=(const PresentRenderPass&) = delete;
			PresentRenderPass(PresentRenderPass&&) = delete;
			PresentRenderPass& operator=(PresentRenderPass&&) = delete;

			RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
		};
	}
}