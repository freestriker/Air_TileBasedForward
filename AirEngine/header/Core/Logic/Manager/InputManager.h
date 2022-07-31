#pragma once
#include <map>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <Qt>

namespace AirEngine
{
	namespace Core
	{
		namespace Logic
		{
			namespace CoreObject
			{
				class Thread;
				class Instance;
			}
			namespace Manager
			{
				enum class InputEventType
				{
					None = 0,                               // invalid event
					//MouseButtonPress = 2,                   // mouse button pressed
					//MouseButtonRelease = 3,                 // mouse button released
					//MouseButtonDblClick = 4,                // mouse button double click
					//MouseMove = 5,                          // mouse move
					KeyPress = 6,                           // key pressed
					KeyRelease = 7,                         // key released
				};
				typedef Qt::Key InputKeyType;
				enum class KeyStatusType
				{
					Pressed,
					Released
				};

				class InputManager final
				{
					friend class CoreObject::Thread;
					friend class CoreObject::Instance;
				private:
					struct KeyEvent
					{
						InputEventType eventType;
						InputKeyType keyType;
					};
					std::vector<KeyEvent> _inputBuffer;
					std::unordered_map< InputKeyType, KeyStatusType> _statusTable;
					std::vector< KeyEvent> _inputQueue;
					std::mutex _managerMutex;

				private:
					InputManager();
					~InputManager();
					InputManager(const InputManager&) = delete;
					InputManager& operator=(const InputManager&) = delete;
					InputManager(InputManager&&) = delete;
					InputManager& operator=(InputManager&&) = delete;

				public:
					void Refresh();
					void Clear();
					void InputKey(InputEventType eventType, InputKeyType key);
					bool KeyUp(InputKeyType key);
					bool KeyDown(InputKeyType key);
					bool KeyAny(InputKeyType key);
					KeyStatusType KeyStatus(InputKeyType key);
				};
			}
		}
	}
}