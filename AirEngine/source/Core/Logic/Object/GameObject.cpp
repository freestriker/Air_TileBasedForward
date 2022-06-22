#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Component.h"
#include <rttr/registration>
#include "Utils/Log.h"
#include "Utils/ChildBrotherTree.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<AirEngine::Core::Logic::Object::GameObject>("AirEngine::Core::Logic::Object::GameObject");
}

AirEngine::Core::Logic::Object::GameObject::GameObject(std::string name)
	: Object()
	, name(name)
	, _timeSqueueComponentsHead()
	, _typeSqueueComponentsHeadMap()
	, transform()
{
	transform._gameObject = this;
}

AirEngine::Core::Logic::Object::GameObject::GameObject()
	: GameObject("New GameObject")
{
}

AirEngine::Core::Logic::Object::GameObject::~GameObject()
{

}

void AirEngine::Core::Logic::Object::GameObject::AddComponent(AirEngine::Core::Logic::Object::Component* targetComponent)
{
	_timeSqueueComponentsHead.Add(targetComponent);
	if (!_typeSqueueComponentsHeadMap.count(targetComponent->_type))
	{
		_typeSqueueComponentsHeadMap[targetComponent->_type] = std::unique_ptr<Utils::CrossLinkableRowHead>(new Utils::CrossLinkableRowHead());
	}
	_typeSqueueComponentsHeadMap[targetComponent->_type]->Add(targetComponent);
	targetComponent->_gameObject = this;
}

void AirEngine::Core::Logic::Object::GameObject::RemoveComponent(AirEngine::Core::Logic::Object::Component* targetComponent)
{
	if (targetComponent->_gameObject != this)
	{
		Utils::Log::Exception("Component do not blong to this GameObject.");
	}

	_timeSqueueComponentsHead.Remove(targetComponent);
	_typeSqueueComponentsHeadMap[targetComponent->_type]->Remove(targetComponent);
	targetComponent->_gameObject = nullptr;

	if (!_typeSqueueComponentsHeadMap[targetComponent->_type]->HaveNode())
	{
		_typeSqueueComponentsHeadMap.erase(targetComponent->_type);
	}

	//if (Logic::Core::Instance::_validComponentInIteration.count(targetComponent))
	//{
	//	Logic::Core::Instance::_validComponentInIteration.erase(targetComponent);
	//}
}

AirEngine::Core::Logic::Object::Component* AirEngine::Core::Logic::Object::GameObject::RemoveComponent(std::string targetTypeName)
{
	return RemoveComponent(rttr::type::get_by_name(targetTypeName));
}

AirEngine::Core::Logic::Object::Component* AirEngine::Core::Logic::Object::GameObject::RemoveComponent(rttr::type targetType)
{
	if (!targetType)
	{
		Utils::Log::Exception("Do not have " + targetType.get_name().to_string() + ".");
	}

	if (!targetType.is_derived_from(Component::COMPONENT_TYPE))
	{
		Utils::Log::Exception(targetType.get_name().to_string() + " is not a component.");
	}

	for (const auto& pair : Component::TYPE_MAP)
	{
		if ((targetType == pair.first || targetType.is_base_of(pair.first)) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			Component* found = static_cast<Component*>(_typeSqueueComponentsHeadMap[pair.second]->GetIterator().Node());
			RemoveComponent(found);
			return found;
		}
		else if (pair.first.is_base_of(targetType) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto iterator = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); iterator.IsValid(); iterator++)
			{
				Component* found = static_cast<Component*>(iterator.Node());
				if (targetType.is_base_of(found->Type()))
				{
					RemoveComponent(found);
					return found;
				}
			}
		}
	}

	Utils::Log::Exception("GameObject " + name + " do not have a " + targetType.get_name().to_string() + " Component.");
}

std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Logic::Object::GameObject::RemoveComponents(std::string targetTypeName)
{
	return RemoveComponents(rttr::type::get_by_name(targetTypeName));
}

std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Logic::Object::GameObject::RemoveComponents(rttr::type targetType)
{
	if (!targetType)
	{
		Utils::Log::Exception("Do not have " + targetType.get_name().to_string() + ".");
	}

	if (!targetType.is_derived_from(Component::COMPONENT_TYPE))
	{
		Utils::Log::Exception(targetType.get_name().to_string() + " is not a component.");
	}

	auto targetComponents = std::vector<Component*>();
	for (const auto& pair : Component::TYPE_MAP)
	{
		if ((targetType == pair.first || targetType.is_base_of(pair.first)) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto itertor = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); itertor.IsValid(); )
			{
				auto foundComponent = static_cast<Component*>(itertor.Node());

				_timeSqueueComponentsHead.Remove(foundComponent);
				itertor = _typeSqueueComponentsHeadMap[pair.second]->Remove(itertor);
				foundComponent->_gameObject = nullptr;

				targetComponents.emplace_back(foundComponent);
			}
			if (!_typeSqueueComponentsHeadMap[pair.second]->HaveNode())
			{
				_typeSqueueComponentsHeadMap.erase(pair.second);
			}
		}
		else if (pair.first.is_base_of(targetType) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto iterator = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); iterator.IsValid(); )
			{
				Component* found = static_cast<Component*>(iterator.Node());
				if (targetType.is_base_of(found->Type()))
				{
					_timeSqueueComponentsHead.Remove(found);
					iterator = _typeSqueueComponentsHeadMap[pair.second]->Remove(iterator);
					found->_gameObject = nullptr;

					targetComponents.emplace_back(found);
				}
			}
			if (!_typeSqueueComponentsHeadMap[pair.second]->HaveNode())
			{
				_typeSqueueComponentsHeadMap.erase(pair.second);
			}
		}
	}

	return targetComponents;
}

AirEngine::Core::Logic::Object::Component* AirEngine::Core::Logic::Object::GameObject::GetComponent(rttr::type targetType)
{
	if (!targetType)
	{
		Utils::Log::Exception("Do not have " + targetType.get_name().to_string() + ".");
	}

	if (!Component::COMPONENT_TYPE.is_base_of(targetType))
	{
		Utils::Log::Exception(targetType.get_name().to_string() + " is not a component.");
	}

	for (const auto& pair : Component::TYPE_MAP)
	{
		if ((targetType == pair.first || targetType.is_base_of(pair.first)) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			auto node = _typeSqueueComponentsHeadMap[pair.second]->GetIterator().Node();
			Component* found = static_cast<Component*>(node);
			return found;
		}
		else if (pair.first.is_base_of(targetType) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto iterator = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); iterator.IsValid(); iterator++)
			{
				Component* found = static_cast<Component*>(iterator.Node());
				if (targetType.is_base_of(found->Type())) return found;
			}
		}
	}

	Utils::Log::Exception("GameObject " + name + " do not have a " + targetType.get_name().to_string() + " Component.");
}

AirEngine::Core::Logic::Object::Component* AirEngine::Core::Logic::Object::GameObject::GetComponent(std::string targetTypeName)
{
	return GetComponent(rttr::type::get_by_name(targetTypeName));
}

std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Logic::Object::GameObject::GetComponents(rttr::type targetType)
{
	if (!targetType)
	{
		Utils::Log::Exception("Do not have " + targetType.get_name().to_string() + ".");
	}

	if (!targetType.is_derived_from(Component::COMPONENT_TYPE))
	{
		Utils::Log::Exception(targetType.get_name().to_string() + " is not a component.");
	}

	auto targetComponents = std::vector<Component*>();

	for (const auto& pair : Component::TYPE_MAP)
	{
		if ((targetType == pair.first || targetType.is_base_of(pair.first)) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto itertor = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); itertor.IsValid(); )
			{
				auto foundComponent = static_cast<Component*>(itertor.Node());

				targetComponents.emplace_back(foundComponent);
			}
		}
		else if (pair.first.is_base_of(targetType) && _typeSqueueComponentsHeadMap.count(pair.second))
		{
			for (auto iterator = _typeSqueueComponentsHeadMap[pair.second]->GetIterator(); iterator.IsValid(); iterator++)
			{
				Component* found = static_cast<Component*>(iterator.Node());
				if (targetType.is_base_of(found->Type()))
				{
					targetComponents.emplace_back(found);
				}
			}
		}
	}

	return targetComponents;
}

std::vector<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Logic::Object::GameObject::GetComponents(std::string targetTypeName)
{
	return GetComponents(rttr::type::get_by_name(targetTypeName));
}

AirEngine::Core::Logic::Object::GameObject* AirEngine::Core::Logic::Object::GameObject::Parent()
{
	auto targetTransform = transform.Parent();
	return targetTransform ? targetTransform->GameObject() : nullptr;
}

AirEngine::Core::Logic::Object::GameObject* AirEngine::Core::Logic::Object::GameObject::Child()
{
	auto targetTransform = transform.Child();
	return targetTransform ? targetTransform->GameObject() : nullptr;
}

AirEngine::Core::Logic::Object::GameObject* AirEngine::Core::Logic::Object::GameObject::Brother()
{
	auto targetTransform = transform.Brother();
	return targetTransform ? targetTransform->GameObject() : nullptr;
}

void AirEngine::Core::Logic::Object::GameObject::AddChild(AirEngine::Core::Logic::Object::GameObject* child)
{
	transform.AddChild(&child->transform);
}

void AirEngine::Core::Logic::Object::GameObject::RemoveChild(AirEngine::Core::Logic::Object::GameObject* child)
{
	if (child->Parent() == this)
	{
		child->RemoveSelf();
	}
}

void AirEngine::Core::Logic::Object::GameObject::RemoveSelf()
{
	static_cast<Utils::ChildBrotherTree<Transform>*>(&transform)->RemoveSelf();
	//if (Logic::Core::Instance::_validGameObjectInIteration.count(this))
	//{
	//	Logic::Core::Instance::_validGameObjectInIteration.erase(this);
	//}
}