#include "Test/QuadMoveBehaviour.h"
#include "Audio/AudioSource.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/AudioClip.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>
#include "Renderer/Renderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::QuadMoveBehaviour>("AirEngine::Test::QuadMoveBehaviour");
}

AirEngine::Test::QuadMoveBehaviour::QuadMoveBehaviour()
{
}

AirEngine::Test::QuadMoveBehaviour::~QuadMoveBehaviour()
{
}

void AirEngine::Test::QuadMoveBehaviour::OnAwake()
{
}

void AirEngine::Test::QuadMoveBehaviour::OnStart()
{

}

void AirEngine::Test::QuadMoveBehaviour::OnUpdate()
{
	float moveSpeed = 1;
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Q) == AirEngine::Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		auto t = GameObject()->transform.Translation();
		t.z += moveSpeed * Core::Logic::CoreObject::Instance::time.DeltaDuration();
		GameObject()->transform.SetTranslation(t);
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_E) == AirEngine::Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		auto t = GameObject()->transform.Translation();
		t.z -= moveSpeed * Core::Logic::CoreObject::Instance::time.DeltaDuration();
		GameObject()->transform.SetTranslation(t);
	}
}

void AirEngine::Test::QuadMoveBehaviour::OnDestroy()
{
}
