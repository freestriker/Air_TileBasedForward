#include "Test/SelfRotateBehaviour.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/Logic/CoreObject/Instance.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::SelfRotateBehaviour>("AirEngine::Test::RotateAboutPointBehaviour");
}

AirEngine::Test::SelfRotateBehaviour::SelfRotateBehaviour(float eulerAngularVelocity)
	: _eulerAngularVelocity(eulerAngularVelocity)
{
}

AirEngine::Test::SelfRotateBehaviour::~SelfRotateBehaviour()
{
}

void AirEngine::Test::SelfRotateBehaviour::OnAwake()
{
}

void AirEngine::Test::SelfRotateBehaviour::OnStart()
{
}

void AirEngine::Test::SelfRotateBehaviour::OnUpdate()
{
	auto rotationAngle = _eulerAngularVelocity * Core::Logic::CoreObject::Instance::time.DeltaDuration();
	auto curRotation = GameObject()->transform.EulerRotation();
	curRotation.x = std::fmod(curRotation.x + rotationAngle * 1.0, 360);
	curRotation.y = std::fmod(curRotation.y + rotationAngle * 0.6, 360);
	curRotation.z = std::fmod(curRotation.z + rotationAngle * 0.3, 360);
	GameObject()->transform.SetEulerRotation(curRotation);
}

void AirEngine::Test::SelfRotateBehaviour::OnDestroy()
{
}
