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
			class TBForwardRenderer final : public Core::Graphic::Rendering::RendererBase
			{
			public:
				enum class TransparentType
				{
					DEPTH_PEELING,
					ALPHA_BUFFER,
					DEPTH_SORT
				};
				class TBForwardRendererData final : public Core::Graphic::Rendering::RendererDataBase
				{
				public:
					CONSTRUCTOR(TBForwardRendererData)
					TransparentType transparentType = TransparentType::DEPTH_PEELING;
					RTTR_ENABLE(Core::Graphic::Rendering::RendererDataBase)
				};

				CONSTRUCTOR(TBForwardRenderer)

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