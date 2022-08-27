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
			namespace RenderPass
			{
				class RenderPassBase;
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
				class RenderPassManager;
				class RenderPassTarget final
				{
					friend class RenderPassManager;
				public:
					Instance::FrameBuffer* FrameBuffer(std::string name);
					Instance::Image* Attachment(std::string name);
					VkExtent2D Extent();
					std::vector<RenderPass::RenderPassBase*>* RenderPasses();
				private:
					std::vector<RenderPass::RenderPassBase*> _passes;
					std::vector<Instance::FrameBuffer*> _frameBuffers;
					std::map<std::string, size_t> _indexMap;
					VkExtent2D _extent;
					std::map<std::string, Instance::Image*> _attachments;
					RenderPassTarget();
					~RenderPassTarget();
					RenderPassTarget(const RenderPassTarget&) = delete;
					RenderPassTarget& operator=(const RenderPassTarget&) = delete;
					RenderPassTarget(RenderPassTarget&&) = delete;
					RenderPassTarget& operator=(RenderPassTarget&&) = delete;
				};
				class RenderPassManager
				{
					friend class CoreObject::Thread;
					friend class CoreObject::Instance;
				private:
					std::mutex _managerMutex;
					std::map<std::string, RenderPass::RenderPassBase*> _renderPasss;
					std::set<RenderPassTarget*> _objects;
					RenderPassManager();
					~RenderPassManager();
				public:
					void AddRenderPass(RenderPass::RenderPassBase* renderPass);
					void RemoveRenderPass(std::string name);
					RenderPass::RenderPassBase& RenderPass(std::string name);
					RenderPassTarget* GetRenderPassObject(std::vector<std::string> renderPasses, std::map<std::string, Instance::Image*> availableAttachments);
					void DestroyRenderPassObject(RenderPassTarget*& renderPassObject);
				};

				class NewRenderPassManager final
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
					NewRenderPassManager();
					~NewRenderPassManager();
					NewRenderPassManager(const NewRenderPassManager&) = delete;
					NewRenderPassManager& operator=(const NewRenderPassManager&) = delete;
					NewRenderPassManager(NewRenderPassManager&&) = delete;
					NewRenderPassManager& operator=(NewRenderPassManager&&) = delete;

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
				Rendering::RenderPassBase* AirEngine::Core::Graphic::Manager::NewRenderPassManager::LoadRenderPass()
				{
					return LoadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string());
				}

				template<typename TRenderPass>
				void AirEngine::Core::Graphic::Manager::NewRenderPassManager::UnloadRenderPass()
				{
					UnloadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string());
				}

			}
		}
	}
}
