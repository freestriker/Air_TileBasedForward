#pragma once
#include "Core/Logic/Object/Object.h"
#include "Utils/CrossLinkableNode.h"
#include <map>
namespace AirEngine
{
	namespace Core
	{
		namespace Logic
		{
			namespace CoreObject
			{
				class Thread;
			}
			namespace Object
			{
				class GameObject;
				class Component
					: public Object
					, private Utils::CrossLinkableNode
				{
					friend class GameObject;
					friend class CoreObject::Thread;
				public:
					enum class ComponentType
					{
						NONE,
						TRANSFORM,
						BEHAVIOUR,
						CAMERA,
						RENDERER,
						LIGHT
					};
				private:
					bool _active;
					bool _neverStarted;
					bool _neverAwaked;
					Component(const Component&) = delete;
					Component& operator=(const Component&) = delete;
					Component(Component&&) = delete;
					Component& operator=(Component&&) = delete;
				protected:
					const static std::map<rttr::type, ComponentType> TYPE_MAP;
					const static rttr::type COMPONENT_TYPE;
					ComponentType _type;
					Logic::Object::GameObject* _gameObject;
					Component();
					Component(ComponentType type);
					virtual ~Component();
					void Awake();
					void Update();
					virtual void OnAwake();
					virtual void OnStart();
					virtual void OnUpdate();
					virtual void OnDestroy();
				public:
					bool Active();
					void SetActive(bool active);
					Logic::Object::GameObject* GameObject();
					ComponentType GetComponentType();
					RTTR_ENABLE(Logic::Object::Object)
				};
			}
		}
	}
}