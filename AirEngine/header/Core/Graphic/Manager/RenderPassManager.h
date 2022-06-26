#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
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
			namespace Instance
			{
				class FrameBuffer;
				class Image;
			}
			namespace Manager
			{
				class RenderPassManager;
				class RenderPassObject final
				{
					friend class RenderPassManager;
				private:
					std::vector<RenderPass::RenderPassBase*> _passes;
					std::vector<Instance::FrameBuffer*> _frameBuffers;
					std::map<std::string, size_t> _indexMap;
					RenderPassObject();
					~RenderPassObject();
					RenderPassObject(const RenderPassObject&) = delete;
					RenderPassObject& operator=(const RenderPassObject&) = delete;
					RenderPassObject(RenderPassObject&&) = delete;
					RenderPassObject& operator=(RenderPassObject&&) = delete;
				};
				class RenderPassManager
				{
					friend class CoreObject::Thread;
					friend class CoreObject::Instance;
				private:
					std::mutex _managerMutex;
					std::map<std::string, RenderPass::RenderPassBase*> _renderPasss;
					std::set<RenderPassObject*> _objects;
					RenderPassManager();
					~RenderPassManager();
				public:
					void AddRenderPass(RenderPass::RenderPassBase* renderPass);
					void RemoveRenderPass(std::string name);
					RenderPass::RenderPassBase& RenderPass(std::string name);
					RenderPassObject* GetRenderPassObject(std::vector<std::string> renderPasses, std::map<std::string, Instance::Image*> availableAttachments);
					void DestroyRenderPassObject(RenderPassObject*& renderPassObject);
				};

			}
		}
	}
}
