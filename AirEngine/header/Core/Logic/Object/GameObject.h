#pragma once
#include "Core/Logic/Object/Object.h"
#include "Utils/CrossLinkableNode.h"
#include "Core/Logic/Object/Transform.h"
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
				class Component;
				class GameObject final : public Object
				{
					friend class CoreObject::Thread;
				private:
					Utils::CrossLinkableColHead _timeSqueueComponentsHead;
					std::map<Component::Component::ComponentType, std::unique_ptr< Utils::CrossLinkableRowHead>> _typeSqueueComponentsHeadMap;


					GameObject(const GameObject&) = delete;
					GameObject& operator=(const GameObject&) = delete;
					GameObject(GameObject&&) = delete;
					GameObject& operator=(GameObject&&) = delete;
				public:
					std::string name;
					Transform transform;

					GameObject(std::string name);
					GameObject();
					virtual ~GameObject();

					//Add
					void AddComponent(Component* component);
					//Remove
					void RemoveComponent(Component* component);
					Component* RemoveComponent(std::string typeName);
					Component* RemoveComponent(rttr::type targetType);
					template<typename TType>
					TType* RemoveComponent();
					std::vector<Component*> RemoveComponents(std::string typeName);
					std::vector<Component*> RemoveComponents(rttr::type targetType);
					template<typename TType>
					std::vector <TType*> RemoveComponents();
					//Get
					Component* GetComponent(rttr::type targetType);
					Component* GetComponent(std::string targetTypeName);
					template<typename TType>
					TType* GetComponent();
					std::vector<Component*> GetComponents(rttr::type targetType);
					std::vector <Component*> GetComponents(std::string targetTypeName);
					AirEngine::Core::Logic::Object::GameObject* Parent();
					AirEngine::Core::Logic::Object::GameObject* Child();
					AirEngine::Core::Logic::Object::GameObject* Brother();
					template<typename TType>
					std::vector <TType*> GetComponents();

					void AddChild(GameObject* child);
					void RemoveChild(GameObject* child);
					void RemoveSelf();

					RTTR_ENABLE(Logic::Object::Object)
				};
				template<typename TType>
				inline TType* GameObject::RemoveComponent()
				{
					return dynamic_cast<TType*>(RemoveComponent(rttr::type::get<TType>()));
				}
				template<typename TType>
				inline std::vector<TType*> GameObject::RemoveComponents()
				{
					auto foundComponents = RemoveComponents(rttr::type::get<TType>());
					auto targetComponents = std::vector<TType*>(foundComponents.size());

					for (uint32_t i = 0; i < targetComponents.size(); i++)
					{
						targetComponents[i] = dynamic_cast<TType*>(foundComponents[i]);
					}

					return targetComponents;
				}
				template<typename TType>
				inline TType* GameObject::GetComponent()
				{
					return dynamic_cast<TType*>(GetComponent(rttr::type::get<TType>()));
				}
				template<typename TType>
				inline std::vector<TType*> GameObject::GetComponents()
				{
					auto foundComponents = GetComponents(rttr::type::get<TType>());
					auto targetComponents = std::vector<TType*>(foundComponents.size());

					for (uint32_t i = 0; i < targetComponents.size(); i++)
					{
						targetComponents[i] = dynamic_cast<TType*>(foundComponents[i]);
					}

					return targetComponents;
				}
			}
		}
	}
}