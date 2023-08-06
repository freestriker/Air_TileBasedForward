#pragma once
#include "Core/Logic/Object/Object.h"
#include <map>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class RendererBase;
				class RenderPipelineBase: public Core::Logic::Object::Object
				{
				private:
					std::map<std::string, RendererBase*> _renderers;
				protected:
					std::string _description;
					RenderPipelineBase();
					virtual ~RenderPipelineBase();
					RenderPipelineBase(const RenderPipelineBase&) = delete;
					RenderPipelineBase& operator=(const RenderPipelineBase&) = delete;
					RenderPipelineBase(RenderPipelineBase&&) = delete;
					RenderPipelineBase& operator=(RenderPipelineBase&&) = delete;

					void UseRenderer(std::string rendererName, RendererBase* renderer);
				public:
					RendererBase* Renderer(std::string rendererName);
					inline const std::map<std::string, RendererBase*>& Renderers() const
					{
						return _renderers;
					}
					inline const std::string& Description()const
					{
						return _description;
					}
					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}