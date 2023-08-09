#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <mutex>
#include <map>
#include <string>
#include <qvulkanwindow.h>

namespace AirEngine
{
	namespace Utils
	{
		class Condition;
	}
	namespace Rendering
	{
		class PresentRenderPass;
	}
	namespace Core
	{
		namespace Logic
		{
			namespace Object
			{
				class Component;
			}
		}
		namespace Graphic
		{
			namespace Instance
			{
				class ImageSampler;
			}
			namespace Rendering
			{
				class FrameBuffer;
				class Shader;
				class Material;
			}
			namespace Manager
			{
				class MemoryManager;
				class RenderPassManager;
				class DescriptorSetManager;
				class LightManager;
				class RenderPipelineManager;
			}
			namespace CoreObject
			{
				class Thread;
				class Instance;
				struct Queue
				{
					friend class Thread;
					friend class Instance;
					uint32_t queueFamilyIndex;
					VkQueue queue;
					std::mutex mutex;
					std::string name;
				private:
					Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue);
				};
				class Instance final
				{
					friend class Thread;
				private:
					static std::map<std::string, Queue*> _queues;
					static QVulkanInstance* _qVulkanInstance;
					static VkInstance _vkInstance;
					static VkPhysicalDevice _vkPhysicalDevice;
					static QVulkanDeviceFunctions* _qDeviceFunctions;
					static VkDevice _vkDevice;

					static Manager::MemoryManager* _memoryManager;
					static Manager::RenderPassManager* _renderPassManager;
					static Manager::DescriptorSetManager* _descriptorSetManager;
					static Manager::LightManager* _lightManager;
					static Manager::RenderPipelineManager* _renderPipelineManager;

					static Utils::Condition* _startPresentCondition;
					static Utils::Condition* _endPresentCondition;
					static Utils::Condition* _startRenderCondition;
					static Utils::Condition* _endRenderCondition;

					static std::vector<Logic::Object::Component*> _lights;
					static std::vector<Logic::Object::Component*> _cameras;
					static std::vector<Logic::Object::Component*> _renderers;

					static double _renderDuration;

					Instance();
					static void Init();
				public:
					static Queue* Queue_(std::string name);
					static QVulkanInstance* QVulkanInstance_();
					static VkInstance VkInstance_();
					static VkPhysicalDevice VkPhysicalDevice_();
					static QVulkanDeviceFunctions* QVulkanDeviceFunctions_();
					static VkDevice VkDevice_();

					static Manager::MemoryManager& MemoryManager();
					static Manager::RenderPassManager& RenderPassManager();
					static Manager::DescriptorSetManager& DescriptorSetManager();
					static Manager::LightManager& LightManager();
					static Manager::RenderPipelineManager& RenderPipelineManager();

					static Utils::Condition& StartPresentCondition();
					static Utils::Condition& EndPresentCondition();
					static Utils::Condition& StartRenderCondition();
					static Utils::Condition& EndRenderCondition();

					static void AddLight(std::vector<Logic::Object::Component*>& lights);
					static void AddCamera(std::vector<Logic::Object::Component*>& cameras);
					static void AddRenderer(std::vector<Logic::Object::Component*>& renderers);
					static void ClearLight();
					static void ClearCamera();
					static void ClearRenderer();

					static inline double RenderDuration()
					{
						return _renderDuration;
					}
				};
			};
		}
	}
}