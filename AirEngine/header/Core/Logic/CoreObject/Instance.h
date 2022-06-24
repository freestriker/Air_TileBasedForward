#pragma once
#include "Core/Logic/Object/GameObject.h"
#include <unordered_set>
#include <Utils/Condition.h>
#include "Utils/Time.h"

namespace AirEngine
{
	namespace Utils
	{
		class Condition;
	}
	namespace Core::Logic
	{
		namespace Object
		{
			class GameObject;
			class Component;
		}
		namespace CoreObject
		{
			class Thread;
			class Instance final
			{
				friend class Thread;
				friend class Logic::Object::GameObject;
			public:
				class RootGameObject final
				{
					friend class Instance;
					friend class Thread;
				private:
					Object::GameObject _gameObject;
					RootGameObject();
					~RootGameObject();
				public:
					inline void AddChild(Object::GameObject* child);
					inline void RemoveChild(Object::GameObject* child);
					inline Object::GameObject* Child();
				};
				class Time
				{
					friend class Instance;
					friend class Thread;
				private:
					Utils::Time _time;
					Time();
					~Time();
					inline void Launch();
					inline void Refresh();
				public:
					inline double DeltaDuration();
					inline double LaunchDuration();
				};
				static RootGameObject rootObject;
				static Time time;
				static void Exit();
				static void WaitExit();
				static bool NeedIterateRenderer();
				static void SetNeedIterateRenderer(bool needIterateRenderer);
			private:
				static Utils::Condition* _exitCondition;
				static bool _needIterateRenderer;
				static std::unordered_set< Object::GameObject*> _validGameObjectInIteration;
				static std::unordered_set< Object::Component*> _validComponentInIteration;
				Instance();
				~Instance();
			};
		}
	}
}

inline void AirEngine::Core::Logic::CoreObject::Instance::RootGameObject::AddChild(Object::GameObject* child)
{
	_gameObject.AddChild(child);
}

inline void AirEngine::Core::Logic::CoreObject::Instance::RootGameObject::RemoveChild(Object::GameObject* child)
{
	_gameObject.RemoveChild(child);
}

inline AirEngine::Core::Logic::Object::GameObject* AirEngine::Core::Logic::CoreObject::Instance::RootGameObject::Child()
{
	return _gameObject.Child();
}
inline void AirEngine::Core::Logic::CoreObject::Instance::Time::Launch()
{
	_time.Launch();
}
inline void AirEngine::Core::Logic::CoreObject::Instance::Time::Refresh()
{
	_time.Refresh();
}
inline double AirEngine::Core::Logic::CoreObject::Instance::Time::DeltaDuration()
{
	return _time.DeltaDuration();
}
inline double AirEngine::Core::Logic::CoreObject::Instance::Time::LaunchDuration()
{
	return _time.LaunchDuration();
}
