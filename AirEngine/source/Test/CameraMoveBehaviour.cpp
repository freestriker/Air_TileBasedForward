#include "Test/CameraMoveBehaviour.h"
#include "Renderer/Renderer.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Asset/TextureCube.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::CameraMoveBehaviour>("AirEngine::Test::CameraMoveBehaviour");
}

AirEngine::Test::CameraMoveBehaviour::CameraMoveBehaviour()
	: _rotation(0)
{
}

AirEngine::Test::CameraMoveBehaviour::~CameraMoveBehaviour()
{
}

void AirEngine::Test::CameraMoveBehaviour::OnAwake()
{
}

void AirEngine::Test::CameraMoveBehaviour::OnStart()
{
	//const double pi = std::acos(-1.0);
	//_rotation = std::fmod((_rotation + 90), 360.0f);
	//float x = std::cos(_rotation / 180 * pi) * 5;
	//float y = -std::sin(_rotation / 180 * pi) * 5;
	//GameObject()->transform.SetTranslation({ x, y, 0 });
	//GameObject()->transform.SetEulerRotation({ 90, 90 - _rotation, -(0) });
}

void AirEngine::Test::CameraMoveBehaviour::OnUpdate()
{
	const double pi = std::acos(-1.0);
	_rotation = std::fmod((_rotation + 15 * Core::Logic::CoreObject::Instance::time.DeltaDuration()), 360.0f);
	float x = std::cos(_rotation / 180 * pi) * 5;
	float y = -std::sin(_rotation / 180 * pi) * 5;
	GameObject()->transform.SetTranslation({ x, y, 0 });
	GameObject()->transform.SetEulerRotation({ 90, 90 - _rotation, -(0) });
}

void AirEngine::Test::CameraMoveBehaviour::OnDestroy()
{
}
