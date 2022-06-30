#pragma once
#include <string>
#include <vulkan/vulkan_core.h>
#include <mutex>
namespace AirEngine
{
	namespace Utils
	{
		class Log
		{
			static std::mutex _mutex;
		public:
			static void Message(std::string info, VkResult logCondition);
			static void Message(std::string info, bool logCondition);
			static void Message(std::string info);

			static void Exception(std::string info, VkResult logCondition);
			static void Exception(std::string info, bool logCondition);
			static void Exception(std::string info);
		};
	}
}
