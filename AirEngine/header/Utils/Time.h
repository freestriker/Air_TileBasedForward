#pragma once
#include <chrono>

namespace AirEngine::Utils
{
	class Time
	{
	private:
		std::chrono::system_clock::time_point _launchTime;
		std::chrono::system_clock::time_point _preTime;
		std::chrono::system_clock::time_point _curTime;
	public:
		Time();
		~Time();
		void Launch();
		void Refresh();
		double DeltaDuration();
		double LaunchDuration();
	};
}