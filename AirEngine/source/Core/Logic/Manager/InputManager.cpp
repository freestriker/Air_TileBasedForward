#include "Core/Logic/Manager/InputManager.h"
#include "Utils/Log.h"

AirEngine::Core::Logic::Manager::InputManager::InputManager()
	: _inputBuffer()
	, _statusTable()
	, _inputQueue()
	, _managerMutex()
{
}

AirEngine::Core::Logic::Manager::InputManager::~InputManager()
{
}

void AirEngine::Core::Logic::Manager::InputManager::Refresh()
{
	std::lock_guard<std::mutex> locker(_managerMutex);
	if (_inputBuffer.size() <= 0) return;
	//Utils::Log::Message("-----Refresh-----");
	for (const auto& event : _inputBuffer)
	{
		_inputQueue.emplace_back(event);
		
		switch (event.eventType)
		{
		case InputEventType::KeyPress:
		{
			_statusTable[event.keyType] = KeyStatusType::Pressed;
			break;
		}
		case InputEventType::KeyRelease:
		{
			_statusTable[event.keyType] = KeyStatusType::Released;
			break;
		}
		}
		//Utils::Log::Message("Key type: " + std::to_string(event.keyType) + ", event type: " + std::to_string(static_cast<int>(event.eventType)) + ".");
	}
	_inputBuffer.clear();
}

void AirEngine::Core::Logic::Manager::InputManager::Clear()
{
	std::lock_guard<std::mutex> locker(_managerMutex);
	_inputQueue.clear();
}

void AirEngine::Core::Logic::Manager::InputManager::InputKey(InputEventType eventType, InputKeyType key)
{
	std::lock_guard<std::mutex> locker(_managerMutex);
	_inputBuffer.emplace_back(KeyEvent{ eventType , key });
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyUp(InputKeyType key)
{
	auto iter = _inputQueue.begin();
	for ( ; iter != _inputQueue.end(); iter++)
	{
		if (iter->keyType == key && iter->eventType == InputEventType::KeyRelease)
		{
			break;
		}
	}

	if (iter != _inputQueue.end())
	{
		_inputQueue.erase(_inputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyDown(InputKeyType key)
{
	auto iter = _inputQueue.begin();
	for (; iter != _inputQueue.end(); iter++)
	{
		if (iter->keyType == key && iter->eventType == InputEventType::KeyPress)
		{
			break;
		}
	}

	if (iter != _inputQueue.end())
	{
		_inputQueue.erase(_inputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyAny(InputKeyType key)
{
	auto iter = _inputQueue.begin();
	for (; iter != _inputQueue.end(); iter++)
	{
		if (iter->keyType == key)
		{
			break;
		}
	}

	if (iter != _inputQueue.end())
	{
		_inputQueue.erase(_inputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

AirEngine::Core::Logic::Manager::KeyStatusType AirEngine::Core::Logic::Manager::InputManager::KeyStatus(InputKeyType key)
{
	return _statusTable.count(key) > 0 ? _statusTable[key] : KeyStatusType::Released;
}
