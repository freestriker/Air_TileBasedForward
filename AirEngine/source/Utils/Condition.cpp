#include "Utils/Condition.h"

AirEngine::Utils::Condition::Condition()
	: _mutex()
	, _ready(false)
	, _conditionVariable()
{
}

AirEngine::Utils::Condition::~Condition()
{
}

void AirEngine::Utils::Condition::Wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_ready)
	{

	}
	else
	{
		_conditionVariable.wait(lock, [this] {return this->_ready; });
	}
	_ready = false;
}

void AirEngine::Utils::Condition::Awake()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_ready = true;
	_conditionVariable.notify_all();
}

bool AirEngine::Utils::Condition::IsReady()
{
	return _ready;
}
