#pragma once
#include "Core/Graphic/Rendering/RendererBase.h"

namespace AirEngine
{
	namespace Renderer
	{
		class Renderer;
	}
	namespace Rendering
	{
		namespace Renderer
		{
			class ForwardRenderer final : public Core::Graphic::Rendering::RendererBase
			{
			public:
				class ForwardRendererData final : public Core::Graphic::Rendering::RendererDataBase
				{
				public:
					ForwardRendererData();
					virtual ~ForwardRendererData();
					ForwardRendererData(const ForwardRendererData&) = delete;
					ForwardRendererData& operator=(const ForwardRendererData&) = delete;
					ForwardRendererData(ForwardRendererData&&) = delete;
					ForwardRendererData& operator=(ForwardRendererData&&) = delete;

					RTTR_ENABLE(Core::Graphic::Rendering::RendererDataBase)
				};

				ForwardRenderer();
				virtual ~ForwardRenderer();
				ForwardRenderer(const ForwardRenderer&) = delete;
				ForwardRenderer& operator=(const ForwardRenderer&) = delete;
				ForwardRenderer(ForwardRenderer&&) = delete;
				ForwardRenderer& operator=(ForwardRenderer&&) = delete;

			private:
				Core::Graphic::Rendering::RendererDataBase* OnCreateRendererData(Camera::CameraBase* camera)override;
				void OnResolveRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera)override;
				void OnDestroyRendererData(Core::Graphic::Rendering::RendererDataBase* rendererData)override;

				virtual void PrepareRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData) override;
				virtual void ExcuteRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*>const* rendererComponents) override;
				virtual void SubmitRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData) override;
				virtual void FinishRenderer(Core::Graphic::Rendering::RendererDataBase* rendererData) override;

				RTTR_ENABLE(Core::Graphic::Rendering::RendererBase)
			};
		}
	}
}