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
					MousePress = 2,							// mouse button pressed
					MouseRelease = 3,						// mouse button released
					KeyPress = 6,                           // key pressed
					KeyRelease = 7,                         // key released
				};
				typedef Qt::Key InputKeyType;
				typedef Qt::MouseButton InputMouseType;
				enum class ButtonStatusType
				{
					Pressed,
					Released
				};

				class InputManager final
				{
					friend class CoreObject::Thread;
					friend class CoreObject::Instance;
				private:
					struct ButtonEvent
					{
						InputEventType eventType;
						union
						{
							InputKeyType keyType;
							InputMouseType mouseType;
						};
					};
					std::vector<ButtonEvent> _keyInputBuffer;
					std::unordered_map< InputKeyType, ButtonStatusType> _keyStatusTable;
					std::vector< ButtonEvent> _keyInputQueue;
					std::mutex _keyInputMutex;

					std::vector<ButtonEvent> _mouseInputBuffer;
					std::unordered_map< InputMouseType, ButtonStatusType> _mouseStatusTable;
					std::vector< ButtonEvent> _mouseInputQueue;
					std::mutex _mouseInputMutex;

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

					///Key
					void InputKey(InputEventType eventType, InputKeyType key);
					void RefreshKey();
					void ClearKey();
					bool KeyUp(InputKeyType key);
					bool KeyDown(InputKeyType key);
					bool KeyAny(InputKeyType key);
					ButtonStatusType KeyStatus(InputKeyType key);

					///Mouse
					void InputMouse(InputEventType eventType, InputMouseType mouse);
					void RefreshMouse();
					void ClearMouse();
					bool MouseUp(InputMouseType mouse);
					bool MouseDown(InputMouseType mouse);
					bool MouseAny(InputMouseType mouse);
					ButtonStatusType MouseStatus(InputMouseType key);

				};
			}
		}
	}
}