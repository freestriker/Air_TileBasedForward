#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <rttr/type.h>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Thread;
				class Instance;
			}
			namespace Rendering
			{
				class RenderPassBase;
			}
			namespace Instance
			{
				class FrameBuffer;
				class Image;
			}
			namespace Manager
			{
				class RenderPassManager final
				{
					friend class CoreObject::Thread;
					friend class CoreObject::Instance;
				private:
					struct RenderPassWrapper
					{
						int refrenceCount = 0;
						Rendering::RenderPassBase* renderPass = nullptr;
					};
					std::mutex _managerMutex;
					std::map<std::string, RenderPassWrapper> _renderPassWrappers;
					RenderPassManager();
					~RenderPassManager();
					RenderPassManager(const RenderPassManager&) = delete;
					RenderPassManager& operator=(const RenderPassManager&) = delete;
					RenderPassManager(RenderPassManager&&) = delete;
					RenderPassManager& operator=(RenderPassManager&&) = delete;

					void CreateRenderPass(Rendering::RenderPassBase* renderPass);
				public:
					Rendering::RenderPassBase* LoadRenderPass(std::string renderPassName);
					template<typename TRenderPass>
					Rendering::RenderPassBase* LoadRenderPass();

					void UnloadRenderPass(std::string renderPassName);
					template<typename TRenderPass>
					void UnloadRenderPass();
					void UnloadRenderPass(Rendering::RenderPassBase* renderPass);

					void Collect();
				};

				template<typename TRenderPass>
				Rendering::RenderPassBase* AirEngine::Core::Graphic::Manager::RenderPassManager::LoadRenderPass()
				{
					return LoadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string());
				}

				template<typename TRenderPass>
				void AirEngine::Core::Graphic::Manager::RenderPassManager::UnloadRenderPass()
				{
					UnloadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string());
				}

			}
		}
	}
}
