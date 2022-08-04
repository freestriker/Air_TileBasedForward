#include "Core/Logic/Manager/InputManager.h"
#include "Utils/Log.h"
#include <qcursor.h>
#include "Core/Graphic/CoreObject/Window.h"

AirEngine::Core::Logic::Manager::InputManager::InputManager()
	: _keyInputBuffer()
	, _keyStatusTable()
	, _keyInputQueue()
	, _keyInputMutex()
	, _mouseInputBuffer()
	, _mouseStatusTable()
	, _mouseInputQueue()
	, _mouseInputMutex()
	, _wheelInputBuffer()
	, _wheelDeltaDegree()
	, _wheelInputMutex()
	, _cursorPosition()
{
}

AirEngine::Core::Logic::Manager::InputManager::~InputManager()
{
}

void AirEngine::Core::Logic::Manager::InputManager::Refresh()
{
	RefreshKey();
	RefreshMouse();
	RefreshWheel();
	RefreshCursor();
}

void AirEngine::Core::Logic::Manager::InputManager::Clear()
{
	ClearKey();
	ClearMouse();
	ClearWheel();
	ClearCursor();
}

void AirEngine::Core::Logic::Manager::InputManager::InputKey(InputEventType eventType, InputKeyType key)
{
	std::lock_guard<std::mutex> locker(_keyInputMutex);
	_keyInputBuffer.emplace_back(ButtonEvent{ eventType , key });
}

void AirEngine::Core::Logic::Manager::InputManager::RefreshKey()
{
	std::lock_guard<std::mutex> locker(_keyInputMutex);
	if (_keyInputBuffer.size() > 0)
	{
		for (const auto& event : _keyInputBuffer)
		{
			_keyInputQueue.emplace_back(event);

			switch (event.eventType)
			{
			case InputEventType::KeyPress:
			{
				_keyStatusTable[event.keyType] = ButtonStatusType::Pressed;
				break;
			}
			case InputEventType::KeyRelease:
			{
				_keyStatusTable[event.keyType] = ButtonStatusType::Released;
				break;
			}
			}
			//Utils::Log::Message("Key type: " + std::to_string(event.keyType) + ", event type: " + std::to_string(static_cast<int>(event.eventType)) + ".");
		}
		_keyInputBuffer.clear();
	}
}

void AirEngine::Core::Logic::Manager::InputManager::ClearKey()
{
	std::lock_guard<std::mutex> locker(_keyInputMutex);
	_keyInputQueue.clear();
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyUp(InputKeyType key)
{
	auto iter = _keyInputQueue.begin();
	for ( ; iter != _keyInputQueue.end(); iter++)
	{
		if (iter->keyType == key && iter->eventType == InputEventType::KeyRelease)
		{
			break;
		}
	}

	if (iter != _keyInputQueue.end())
	{
		_keyInputQueue.erase(_keyInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyDown(InputKeyType key)
{
	auto iter = _keyInputQueue.begin();
	for (; iter != _keyInputQueue.end(); iter++)
	{
		if (iter->keyType == key && iter->eventType == InputEventType::KeyPress)
		{
			break;
		}
	}

	if (iter != _keyInputQueue.end())
	{
		_keyInputQueue.erase(_keyInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::KeyAny(InputKeyType key)
{
	auto iter = _keyInputQueue.begin();
	for (; iter != _keyInputQueue.end(); iter++)
	{
		if (iter->keyType == key)
		{
			break;
		}
	}

	if (iter != _keyInputQueue.end())
	{
		_keyInputQueue.erase(_keyInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

AirEngine::Core::Logic::Manager::ButtonStatusType AirEngine::Core::Logic::Manager::InputManager::KeyStatus(InputKeyType key)
{
	return _keyStatusTable.count(key) > 0 ? _keyStatusTable[key] : ButtonStatusType::Released;
}

void AirEngine::Core::Logic::Manager::InputManager::InputMouse(InputEventType eventType, InputMouseType mouse)
{
	std::lock_guard<std::mutex> locker(_mouseInputMutex);
	_mouseInputBuffer.emplace_back(ButtonEvent{ eventType , static_cast<InputKeyType>(mouse) });
}

void AirEngine::Core::Logic::Manager::InputManager::RefreshMouse()
{
	std::lock_guard<std::mutex> locker(_mouseInputMutex);
	if (_mouseInputBuffer.size() > 0)
	{
		for (const auto& event : _mouseInputBuffer)
		{
			_mouseInputQueue.emplace_back(event);

			switch (event.eventType)
			{
			case InputEventType::MousePress:
			{
				_mouseStatusTable[event.mouseType] = ButtonStatusType::Pressed;
				break;
			}
			case InputEventType::MouseRelease:
			{
				_mouseStatusTable[event.mouseType] = ButtonStatusType::Released;
				break;
			}
			}
			//Utils::Log::Message("Mouse type: " + std::to_string(event.mouseType) + ", event type: " + std::to_string(static_cast<int>(event.eventType)) + ".");
		}
		_mouseInputBuffer.clear();
	}
}

void AirEngine::Core::Logic::Manager::InputManager::ClearMouse()
{
	std::lock_guard<std::mutex> locker(_mouseInputMutex);
	_mouseInputQueue.clear();
}

bool AirEngine::Core::Logic::Manager::InputManager::MouseUp(InputMouseType mouse)
{
	auto iter = _mouseInputQueue.begin();
	for (; iter != _mouseInputQueue.end(); iter++)
	{
		if (iter->mouseType == mouse && iter->eventType == InputEventType::MouseRelease)
		{
			break;
		}
	}

	if (iter != _mouseInputQueue.end())
	{
		_mouseInputQueue.erase(_mouseInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::MouseDown(InputMouseType mouse)
{
	auto iter = _mouseInputQueue.begin();
	for (; iter != _mouseInputQueue.end(); iter++)
	{
		if (iter->mouseType == mouse && iter->eventType == InputEventType::MousePress)
		{
			break;
		}
	}

	if (iter != _mouseInputQueue.end())
	{
		_mouseInputQueue.erase(_mouseInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool AirEngine::Core::Logic::Manager::InputManager::MouseAny(InputMouseType mouse)
{
	auto iter = _mouseInputQueue.begin();
	for (; iter != _mouseInputQueue.end(); iter++)
	{
		if (iter->mouseType == mouse)
		{
			break;
		}
	}

	if (iter != _mouseInputQueue.end())
	{
		_mouseInputQueue.erase(_mouseInputQueue.begin(), iter);
		return true;
	}
	else
	{
		return false;
	}
}

AirEngine::Core::Logic::Manager::ButtonStatusType AirEngine::Core::Logic::Manager::InputManager::MouseStatus(InputMouseType mouse)
{
	return _mouseStatusTable.count(mouse) > 0 ? _mouseStatusTable[mouse] : ButtonStatusType::Released;
}

void AirEngine::Core::Logic::Manager::InputManager::InputWheel(float deltaDegree)
{
	std::lock_guard<std::mutex> locker(_wheelInputMutex);
	_wheelInputBuffer += deltaDegree;
}

void AirEngine::Core::Logic::Manager::InputManager::RefreshWheel()
{
	{
		std::lock_guard<std::mutex> locker(_wheelInputMutex);
		_wheelDeltaDegree = _wheelInputBuffer;
		_wheelInputBuffer = 0.0f;
	}
	//if (_wheelDeltaDegree != 0.0f)
	//{
	//	Utils::Log::Message("Wheel delta degree: " + std::to_string(_wheelDeltaDegree) + ".");
	//}
}

void AirEngine::Core::Logic::Manager::InputManager::ClearWheel()
{
	std::lock_guard<std::mutex> locker(_wheelInputMutex);
	_wheelDeltaDegree = 0.0f;
}

float AirEngine::Core::Logic::Manager::InputManager::WheelDeltaDegree()
{
	return _wheelDeltaDegree;
}

bool AirEngine::Core::Logic::Manager::InputManager::WheelScrolled()
{
	return _wheelDeltaDegree != 0.0f;
}

void AirEngine::Core::Logic::Manager::InputManager::SetCursor(glm::ivec2 position)
{
	QCursor::setPos(Core::Graphic::CoreObject::Window::VulkanWindow_()->mapToGlobal(QPoint{ position .x, position .y}));
	_cursorPosition = position;
}

void AirEngine::Core::Logic::Manager::InputManager::RefreshCursor()
{
	auto relativePos = Core::Graphic::CoreObject::Window::VulkanWindow_()->mapFromGlobal(QCursor::pos());
	_cursorPosition = { relativePos.x(), relativePos.y() };
	//Utils::Log::Message("Mouse relative position: (" + std::to_string(relativePos.x()) + ", " + std::to_string(relativePos.y()) + ").");
}

void AirEngine::Core::Logic::Manager::InputManager::ClearCursor()
{
	_cursorPosition = { 0, 0 };
}

glm::ivec2 AirEngine::Core::Logic::Manager::InputManager::Cursor()
{
	return _cursorPosition;
}
