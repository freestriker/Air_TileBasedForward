#include "Utils/Time.h"

AirEngine::Utils::Time::Time()
	: _launchTime()
	, _preTime()
	, _curTime()
{
}

AirEngine::Utils::Time::~Time()
{
}

void AirEngine::Utils::Time::Launch()
{
	_launchTime = std::chrono::system_clock::now();
	_preTime = _launchTime;
	_curTime = _launchTime;
}

void AirEngine::Utils::Time::Refresh()
{
	_preTime = _curTime;
	_curTime = std::chrono::system_clock::now();
}

double AirEngine::Utils::Time::DeltaDuration()
{
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(_curTime - _preTime);
	return double(duration.count()) / 1000000000;
}

double AirEngine::Utils::Time::LaunchDuration()
{
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(_curTime - _launchTime);
	return double(duration.count()) / 1000000000;
}
