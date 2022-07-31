#include "Test/AudioSourceBehaviour.h"
#include "Audio/AudioSource.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/AudioClip.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::AudioSourceBehaviour>("AirEngine::Test::AudioSourceBehaviour");
}

AirEngine::Test::AudioSourceBehaviour::AudioSourceBehaviour(std::string audioClipPath)
	: _audioClipPath(audioClipPath)
{
}

AirEngine::Test::AudioSourceBehaviour::~AudioSourceBehaviour()
{
}

void AirEngine::Test::AudioSourceBehaviour::OnAwake()
{
}

void AirEngine::Test::AudioSourceBehaviour::OnStart()
{
	auto clip = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::AudioClip>("..\\Asset\\Audio\\IWouldGiveYouAnything.flac");
	double time = Core::Logic::CoreObject::Instance::time.LaunchDuration();
	double gain = std::abs(2.0 * std::fmod(time, 10.0f) / 10.f - 0.5f);
	GameObject()->GetComponent<Audio::AudioSource>()->SetGain(gain);
	GameObject()->GetComponent<Audio::AudioSource>()->Play(clip);
}

void AirEngine::Test::AudioSourceBehaviour::OnUpdate()
{
	auto audioSource = GameObject()->GetComponent<Audio::AudioSource>();
	//double time = Core::Logic::CoreObject::Instance::time.LaunchDuration();
	//double gain = std::abs(2.0 * std::fmod(time, 10.0f) / 10.f - 0.5f);
	auto gain = audioSource->GetGain();
	float deltaVolum = 0.05;
	if (Core::Logic::CoreObject::Instance::InputManager().KeyDown(Core::Logic::Manager::InputKeyType::Key_Down))
	{
		gain -= deltaVolum;
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_Up))
	{
		gain += deltaVolum;
	}
	audioSource->SetGain(std::clamp(gain, 0.0f, 1.0f));
}

void AirEngine::Test::AudioSourceBehaviour::OnDestroy()
{
}
