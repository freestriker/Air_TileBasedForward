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
			class OceanRenderer final : public Core::Graphic::Rendering::RendererBase
			{
			public:
				class OceanRendererData final : public Core::Graphic::Rendering::RendererDataBase
				{
				public:
					CONSTRUCTOR(OceanRendererData)
					RTTR_ENABLE(Core::Graphic::Rendering::RendererDataBase)
				};

				CONSTRUCTOR(OceanRenderer)

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