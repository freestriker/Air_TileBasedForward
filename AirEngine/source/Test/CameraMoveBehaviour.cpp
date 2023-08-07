#include "Test/CameraMoveBehaviour.h"
#include "Renderer/Renderer.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Core/Logic/Manager/InputManager.h"
#include "Utils/Log.h"
#include "Core/Graphic/CoreObject/Window.h"

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
	/////Clockwise
	//_rotation = std::fmod((_rotation + 270), 360.0f);
	//float x = std::cos(_rotation / 180 * pi) * 5;
	//float z = std::sin(_rotation / 180 * pi) * 5;
	//GameObject()->transform.SetTranslation({ x, 0, z });
	//GameObject()->transform.SetEulerRotation({ 0, 90 - _rotation, 0 });
	GameObject()->transform.SetEulerRotation({ 0, 0, 0 });
}

void AirEngine::Test::CameraMoveBehaviour::OnUpdate()
{
	const double pi = std::acos(-1.0);
	///Clockwise
	_rotation = std::fmod((_rotation + 15 * Core::Logic::CoreObject::Instance::time.DeltaDuration()), 360.0f);
	float x = std::cos(_rotation / 180 * pi) * 5;
	float z = std::sin(_rotation / 180 * pi) * 5;
	//GameObject()->transform.SetTranslation({ 0, 0, 0 });
	//GameObject()->transform.SetEulerRotation({ 0, 90 - _rotation, 0 });

	//auto&& cursor = Core::Logic::CoreObject::Instance::InputManager().Cursor();
	//Utils::Log::Message(std::to_string(cursor.x) + " " + std::to_string(cursor.y));
	//static glm::vec2 preCursorPos = Core::Logic::CoreObject::Instance::InputManager().Cursor();
	//glm::vec2 curCursorPos = Core::Logic::CoreObject::Instance::InputManager().Cursor();
	//glm::vec2 windowSize = glm::vec2(Core::Graphic::CoreObject::Window::VulkanWindow_()->size().width(), Core::Graphic::CoreObject::Window::VulkanWindow_()->size().height());
	//if (
	//	0 <= preCursorPos.x && preCursorPos.x < windowSize.x &&
	//	0 <= preCursorPos.x && preCursorPos.x < windowSize.x &&
	//	0 <= preCursorPos.x && preCursorPos.x < windowSize.x &&
	//	0 <= preCursorPos.x && preCursorPos.x < windowSize.x
	//)
	//{

	//}
	const float deltaDuration = Core::Logic::CoreObject::Instance::time.DeltaDuration();
	const double rotationSpeed = 20;
	glm::vec3 deltaRotation = {};
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Left) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaRotation.y = rotationSpeed * deltaDuration;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Right) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaRotation.y = -rotationSpeed * deltaDuration;
	}
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Up) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaRotation.x = rotationSpeed * deltaDuration;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Down) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaRotation.x = -rotationSpeed * deltaDuration;
	}
	GameObject()->transform.SetEulerRotation(GameObject()->transform.EulerRotation() + deltaRotation);

	const float translationSpeed = 2;
	glm::vec3 deltaTranslation = {};
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_W) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation = (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
		GameObject()->transform.SetTranslation(GameObject()->transform.Translation() + deltaTranslation);
		Utils::Log::Message(std::to_string(GameObject()->transform.Translation().x) + " " + std::to_string(GameObject()->transform.Translation().z));
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_S) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation = -(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
		GameObject()->transform.SetTranslation(GameObject()->transform.Translation() + deltaTranslation);
	}
}

void AirEngine::Test::CameraMoveBehaviour::OnDestroy()
{
}
