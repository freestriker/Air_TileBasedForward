#pragma once
#include "Core/Logic/Object/Object.h"
#include <vector>

namespace AirEngine
{
	namespace Renderer
	{
		class Renderer;
	}
	namespace Camera
	{
		class CameraBase;
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

					virtual RenderFeatureDataBase* OnCreateRenderFeatureData(Camera::CameraBase* camera) = 0;
					virtual void OnResolveRenderFeatureData(RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera) = 0;
					virtual void OnDestroyRenderFeatureData(RenderFeatureDataBase* renderFeatureData) = 0;

					virtual void OnPrepare(RenderFeatureDataBase* renderFeatureData) = 0;
					virtual void OnExcute(Camera::CameraBase* camera, RenderFeatureDataBase* renderFeatureData, Command::CommandBuffer* commandBuffer, std::vector<Renderer::Renderer*>const * rendererComponents) = 0;
					virtual void OnSubmit(RenderFeatureDataBase* renderFeatureData, Command::CommandBuffer* commandBuffer) = 0;
					virtual void OnFinish(RenderFeatureDataBase* renderFeatureData) = 0;

					RTTR_ENABLE(Core::Logic::Object::Object)
				};
			}
		}
	}
}