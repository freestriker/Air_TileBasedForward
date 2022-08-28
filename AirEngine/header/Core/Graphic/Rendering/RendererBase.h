#pragma once
#include "Core/Logic/Object/Object.h"
#include <map>

namespace AirEngine
{
	namespace Camera
	{
		class CameraBase;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class RenderFeatureBase;
				class RenderFeatureDataBase;
				class RendererBase;
				class RendererDataBase : public Core::Logic::Object::Object
				{
					friend class RendererBase;
				private:
					std::map<std::string, RenderFeatureDataBase*> _renderFeatureDatas;
				protected:
					RendererDataBase();
					virtual ~RendererDataBase();
					RendererDataBase(const RendererDataBase&) = delete;
					RendererDataBase& operator=(const RendererDataBase&) = delete;
					RendererDataBase(RendererDataBase&&) = delete;
					RendererDataBase& operator=(RendererDataBase&&) = delete;
				public:
					RenderFeatureDataBase* RenderFeatureData(std::string renderFeatureName);

					RTTR_ENABLE(Core::Logic::Object::Object)
				};

				class RendererBase : public Core::Logic::Object::Object
				{
				private:
					std::map<std::string, RenderFeatureBase*> _renderFeatures;	
				protected:
					RendererBase();
					virtual ~RendererBase();
					RendererBase(const RendererBase&) = delete;
					RendererBase& operator=(const RendererBase&) = delete;
					RendererBase(RendererBase&&) = delete;
					RendererBase& operator=(RendererBase&&) = delete; 

					virtual RendererDataBase* OnCreateRendererData(Camera::CameraBase* camera) = 0;
					virtual void OnResolveRendererData(RendererDataBase* rendererData, Camera::CameraBase* camera) = 0;
					virtual void OnDestroyRendererData(RendererDataBase* rendererData) = 0;

					void UseRenderFeature(std::string name, RenderFeatureBase* renderFeature);
				public:
					RendererDataBase* CreateRendererData(Camera::CameraBase* camera);
					void DestroyRendererData(RendererDataBase* rendererData);

					RenderFeatureBase* RenderFeature(std::string name);

					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}