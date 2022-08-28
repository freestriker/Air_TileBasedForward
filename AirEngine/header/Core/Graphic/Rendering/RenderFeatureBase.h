#pragma once
#include "Core/Logic/Object/Object.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class RendererBase;
				class RendererDataBase;
				class RenderFeatureDataBase : public Core::Logic::Object::Object
				{
				protected:
					RenderFeatureDataBase();
					virtual ~RenderFeatureDataBase();
					RenderFeatureDataBase(const RenderFeatureDataBase&) = delete;
					RenderFeatureDataBase& operator=(const RenderFeatureDataBase&) = delete;
					RenderFeatureDataBase(RenderFeatureDataBase&&) = delete;
					RenderFeatureDataBase& operator=(RenderFeatureDataBase&&) = delete;

					RTTR_ENABLE(Core::Logic::Object::Object)
				};
				class RenderFeatureBase : public Core::Logic::Object::Object
				{
					friend class RendererBase;
				protected:
					RenderFeatureBase();
					virtual ~RenderFeatureBase();
					RenderFeatureBase(const RenderFeatureBase&) = delete;
					RenderFeatureBase& operator=(const RenderFeatureBase&) = delete;
					RenderFeatureBase(RenderFeatureBase&&) = delete;
					RenderFeatureBase& operator=(RenderFeatureBase&&) = delete;

					virtual RenderFeatureDataBase* OnCreateRenderFeatureData() = 0;
					virtual void OnDestroyRenderFeatureData(RenderFeatureDataBase* renderFeatureData) = 0;

					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}