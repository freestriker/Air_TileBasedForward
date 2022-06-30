#include "Utils/Log.h"
#include <qdebug.h>

std::mutex AirEngine::Utils::Log::_mutex = std::mutex();

void AirEngine::Utils::Log::Message(std::string info, VkResult logCondition)
{
	if (logCondition != VkResult::VK_SUCCESS)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		std::string s = "Message: " + info + " Error code: " + std::to_string(static_cast<int>(logCondition)) + ".";
		qDebug() << QString::fromStdString(s);
	}
}

void AirEngine::Utils::Log::Message(std::string info, bool logCondition)
{
	if (logCondition)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		std::string s = "Message: " + info + " Error code: " + std::to_string(static_cast<int>(logCondition)) + ".";
		qDebug() << QString::fromStdString(s);
	}
}

void AirEngine::Utils::Log::Message(std::string info)
{
	std::unique_lock<std::mutex> lock(_mutex);
	std::string s = "Message: " + info;
	qDebug() << QString::fromStdString(s);
}

void AirEngine::Utils::Log::Exception(std::string info, VkResult logCondition)
{
	if (logCondition != VkResult::VK_SUCCESS)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		std::string s = "Exception: " + info + " Error code: " + std::to_string(static_cast<int>(logCondition)) + ".";
		qWarning() << QString::fromStdString(s);
	}
}

void AirEngine::Utils::Log::Exception(std::string info, bool logCondition)
{
	if (logCondition)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		std::string s = "Exception: " + info + " Error code: " + std::to_string(static_cast<int>(logCondition)) + ".";
		qWarning() << QString::fromStdString(s);
	}
}

void AirEngine::Utils::Log::Exception(std::string info)
{
	std::unique_lock<std::mutex> lock(_mutex);
	std::string s = "Exception: " + info;
	qWarning() << QString::fromStdString(s);
}
