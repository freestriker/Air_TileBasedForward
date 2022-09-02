#pragma once
#include <map>
#include <mutex>

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
				class RenderPipelineBase;
				class RendererBase;
				class RendererDataBase;
			}
			namespace Manager
			{
				class RenderPipelineManager final
				{
				public:
					RenderPipelineManager();
					~RenderPipelineManager();
					RenderPipelineManager(const RenderPipelineManager&) = delete;
					RenderPipelineManager& operator=(const RenderPipelineManager&) = delete;
					RenderPipelineManager(RenderPipelineManager&&) = delete;
					RenderPipelineManager& operator=(RenderPipelineManager&&) = delete;

					Rendering::RenderPipelineBase* SwitchRenderPipeline(Rendering::RenderPipelineBase* renderPipeline);
					void CreateRendererData(Camera::CameraBase* camera);
					void RefreshRendererData(Camera::CameraBase* camera);
					void DestroyRendererData(Camera::CameraBase* camera);
					Rendering::RenderPipelineBase* RenderPipeline();
					Rendering::RendererBase* Renderer(std::string rendererName);
					Rendering::RendererDataBase* RendererData(Camera::CameraBase* camera);

				private:
					std::mutex _managerMutex;
					Rendering::RenderPipelineBase* _renderPipeline;
					std::map<Camera::CameraBase*, Rendering::RendererDataBase*> _rendererDatas;
				};
			}
		}
	}
}