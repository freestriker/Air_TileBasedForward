#pragma once
#include "Core/Logic/Object/Object.h"
#include <map>
#include <future>

namespace AirEngine
{
	namespace Camera
	{
		class CameraBase;
	}
	namespace Renderer
	{
		class Renderer;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandBuffer;
			}
			namespace Rendering
			{
				class RenderFeatureBase;
				class RenderFeatureDataBase;
				class RendererBase;
				class RendererDataBase : public Core::Logic::Object::Object
				{
					friend class RendererBase;
				private:
					struct RenderFeatureWrapper
					{
						RenderFeatureDataBase* renderFeatureData;
						std::future<void> excuteTask;
						Command::CommandBuffer* commandBuffer;
					};
					std::map<std::string, RenderFeatureWrapper> _renderFeatureWrappers;
				protected:
					RendererDataBase();
					virtual ~RendererDataBase();
					RendererDataBase(const RendererDataBase&) = delete;
					RendererDataBase& operator=(const RendererDataBase&) = delete;
					RendererDataBase(RendererDataBase&&) = delete;
					RendererDataBase& operator=(RendererDataBase&&) = delete;
				public:
					RenderFeatureDataBase* RenderFeatureData(std::string renderFeatureName);
					template<typename TRenderFeatureData>
					TRenderFeatureData* RenderFeatureData(std::string renderFeatureName);

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

					void UseRenderFeature(std::string renderFeatureName, RenderFeatureBase* renderFeature);
					void PrepareRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData);
					void ExcuteRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<Renderer::Renderer*>const* rendererComponents);
					void SubmitRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData);
					void FinishRenderFeature(std::string renderFeatureName, RendererDataBase* rendererData);

				public:
					RendererDataBase* CreateRendererData(Camera::CameraBase* camera);
					void DestroyRendererData(RendererDataBase* rendererData);

					virtual void PrepareRenderer(RendererDataBase* rendererData) = 0;
					virtual void ExcuteRenderer(RendererDataBase* rendererData, Camera::CameraBase* camera, std::vector<Renderer::Renderer*>const* rendererComponents) = 0;
					virtual void SubmitRenderer(RendererDataBase* rendererData) = 0;
					virtual void FinishRenderer(RendererDataBase* rendererData) = 0;
					RenderFeatureBase* RenderFeature(std::string renderFeatureName);

					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}

template<typename TRenderFeatureData>
TRenderFeatureData* AirEngine::Core::Graphic::Rendering::RendererDataBase::RenderFeatureData(std::string renderFeatureName)
{
	return static_cast<TRenderFeatureData*>(RenderFeatureData(renderFeatureName));
}
