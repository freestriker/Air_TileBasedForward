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
	namespace Core
	{
		namespace Graphic
		{
			namespace Manager
			{
				class MemoryManager;
			}
			namespace CoreObject
			{
				class Thread;
				class VulkanInstance;
				struct Queue
				{
					friend class Thread;
					friend class VulkanInstance;
					uint32_t queueFamilyIndex;
					VkQueue queue;
					std::mutex mutex;
					std::string name;
				private:
					Queue(std::string name, uint32_t queueFamilyIndex, VkQueue queue);
				};
				class VulkanInstance final
				{
					friend class Thread;
				private:
					static std::map<std::string, Queue*> _queues;
					static QVulkanInstance* _qVulkanInstance;
					static VkInstance _vkInstance;
					static VkPhysicalDevice _vkPhysicalDevice;
					static QVulkanDeviceFunctions* _qDeviceFunctions;
					static VkDevice _vkDevice;

					static  Manager::MemoryManager* _memoryManager;
					VulkanInstance();
					static void Init();
				public:
					static const Queue* GetQueue(std::string name);
					static QVulkanInstance* QVulkanInstance_();
					static VkInstance VulkanInstance_();
					static VkPhysicalDevice VulkanPhysicalDevice_();
					static QVulkanDeviceFunctions* VulkanDeviceFunctions_();
					static VkDevice VulkanDevice_();

					static Manager::MemoryManager& MemoryManager();
				};
			}
		}
	}
}