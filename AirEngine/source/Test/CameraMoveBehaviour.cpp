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
glm::vec3 ToEulerAngles(glm::quat q) {
	glm::vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
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
	const double rotationSpeed = 20.0 / 180.0 * pi;
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

	auto&& src = deltaRotation;
	glm::quat quat = glm::quat(glm::vec3(0, src.y, 0)) * glm::quat(glm::vec3(src.x, 0, 0));
	auto&& delta = ToEulerAngles(quat);

	GameObject()->transform.SetRotation(GameObject()->transform.Rotation() + delta);

	const float translationSpeed = 2;
	glm::vec3 deltaTranslation = {};
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_W) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_S) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 1, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_A) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(-1, 0, 0, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_D) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(1, 0, 0, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Q) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 1, 0, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_E) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		deltaTranslation += (glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, -1, 0, 1)) - glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1))) * translationSpeed * deltaDuration;
	}
	GameObject()->transform.SetTranslation(GameObject()->transform.Translation() + deltaTranslation);
}

void AirEngine::Test::CameraMoveBehaviour::OnDestroy()
{
}
