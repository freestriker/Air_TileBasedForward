#include "Core/Logic/CoreObject/Instance.h"
#include "Utils/Condition.h"
#include "Core/Logic/Manager/InputManager.h"

AirEngine::Core::Logic::CoreObject::Instance::RootGameObject AirEngine::Core::Logic::CoreObject::Instance::rootObject = AirEngine::Core::Logic::CoreObject::Instance::RootGameObject();
AirEngine::Utils::Condition* AirEngine::Core::Logic::CoreObject::Instance::_exitCondition = nullptr;
std::unordered_set<AirEngine::Core::Logic::Object::GameObject*> AirEngine::Core::Logic::CoreObject::Instance::_validGameObjectInIteration = std::unordered_set<AirEngine::Core::Logic::Object::GameObject*>();
std::unordered_set<AirEngine::Core::Logic::Object::Component*> AirEngine::Core::Logic::CoreObject::Instance::_validComponentInIteration = std::unordered_set<AirEngine::Core::Logic::Object::Component*>();
AirEngine::Core::Logic::CoreObject::Instance::Time AirEngine::Core::Logic::CoreObject::Instance::time = AirEngine::Core::Logic::CoreObject::Instance::Time();
bool AirEngine::Core::Logic::CoreObject::Instance::_needIterateRenderer = false;
AirEngine::Core::Logic::Manager::InputManager* AirEngine::Core::Logic::CoreObject::Instance::_inputManager = nullptr;

void AirEngine::Core::Logic::CoreObject::Instance::Init()
{
	_inputManager = new Manager::InputManager();
	_exitCondition = new AirEngine::Utils::Condition();
	_needIterateRenderer = false;
	time.Launch();
}

void AirEngine::Core::Logic::CoreObject::Instance::Exit()
{
	_exitCondition->Awake();
}

void AirEngine::Core::Logic::CoreObject::Instance::WaitExit()
{
	_exitCondition->Wait();
}

bool AirEngine::Core::Logic::CoreObject::Instance::NeedIterateRenderer()
{
	return _needIterateRenderer;
}

void AirEngine::Core::Logic::CoreObject::Instance::SetNeedIterateRenderer(bool needIterateRenderer)
{
	_needIterateRenderer = needIterateRenderer;
}

AirEngine::Core::Logic::Manager::InputManager& AirEngine::Core::Logic::CoreObject::Instance::InputManager()
{
	return *_inputManager;
}

AirEngine::Core::Logic::CoreObject::Instance::Instance()
{
}

AirEngine::Core::Logic::CoreObject::Instance::~Instance()
{
}

AirEngine::Core::Logic::CoreObject::Instance::RootGameObject::RootGameObject()
	: _gameObject("RootGameObject")
{
}

AirEngine::Core::Logic::CoreObject::Instance::RootGameObject::~RootGameObject()
{
}

AirEngine::Core::Logic::CoreObject::Instance::Time::Time()
	: _time()
{
}

AirEngine::Core::Logic::CoreObject::Instance::Time::~Time()
{
}