#include "Core/Logic/Object/Component.h"
#include "Core/Logic/Object/GameObject.h"
#include <rttr/registration>
#include "Core/Logic/Object/Transform.h"
#include "Renderer/Renderer.h"
#include "Behaviour/Behaviour.h"
#include "Camera/CameraBase.h"
#include "Audio/AudioListener.h"
#include "Audio/AudioSource.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::Logic::Object::Component>("AirEngine::Core::Logic::Object::Component");
}

const std::map<rttr::type, AirEngine::Core::Logic::Object::Component::ComponentType> AirEngine::Core::Logic::Object::Component::TYPE_MAP =
std::map<rttr::type, AirEngine::Core::Logic::Object::Component::ComponentType>
({
	{rttr::type::get<Transform>(), AirEngine::Core::Logic::Object::Component::ComponentType::TRANSFORM},
	{rttr::type::get<Behaviour::Behaviour>(), Component::ComponentType::BEHAVIOUR},
	{rttr::type::get<Camera::CameraBase>(), Component::ComponentType::CAMERA},
	{rttr::type::get<Renderer::Renderer>(), Component::ComponentType::RENDERER},
	{rttr::type::get<Audio::AudioListener>(), Component::ComponentType::AUDIO_LISTENER},
	{rttr::type::get<Audio::AudioSource>(), Component::ComponentType::AUDIO_SOURCE},
});

const rttr::type AirEngine::Core::Logic::Object::Component::COMPONENT_TYPE = rttr::type::get< AirEngine::Core::Logic::Object::Component>();

AirEngine::Core::Logic::Object::Component::Component()
	: Component(ComponentType::NONE)
{
}

AirEngine::Core::Logic::Object::Component::Component(ComponentType type)
	: _type(type)
	, Object()
	, Utils::CrossLinkableNode()
	, _gameObject(nullptr)
	, _active(true)
	, _neverStarted(true)
	, _neverAwaked(true)
{
}

AirEngine::Core::Logic::Object::Component::~Component()
{
}

void AirEngine::Core::Logic::Object::Component::Awake()
{
	_neverStarted = true;
	if (_neverAwaked)
	{
		_neverAwaked = false;
		OnAwake();
	}
}

void AirEngine::Core::Logic::Object::Component::Update()
{
	if (_neverStarted)
	{
		_neverStarted = false;
		OnStart();
	}
	OnUpdate();
}

void AirEngine::Core::Logic::Object::Component::OnAwake()
{
}

void AirEngine::Core::Logic::Object::Component::OnStart()
{
}

void AirEngine::Core::Logic::Object::Component::OnUpdate()
{
}

void AirEngine::Core::Logic::Object::Component::OnDestroy()
{
}

bool AirEngine::Core::Logic::Object::Component::Active()
{
	return _active;
}

void AirEngine::Core::Logic::Object::Component::SetActive(bool active)
{
	_active = active;
}

AirEngine::Core::Logic::Object::GameObject* AirEngine::Core::Logic::Object::Component::GameObject()
{
	return _gameObject;
}

AirEngine::Core::Logic::Object::Component::ComponentType AirEngine::Core::Logic::Object::Component::GetComponentType()
{
	return _type;
}
