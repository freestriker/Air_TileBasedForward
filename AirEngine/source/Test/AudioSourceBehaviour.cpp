#include "Test/AudioSourceBehaviour.h"
#include "Audio/AudioSource.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/AudioClip.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>

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
	double time = Core::Logic::CoreObject::Instance::time.LaunchDuration();
	double gain = std::abs(2.0 * std::fmod(time, 10.0f) / 10.f - 0.5f);
	GameObject()->GetComponent<Audio::AudioSource>()->SetGain(gain);
}

void AirEngine::Test::AudioSourceBehaviour::OnDestroy()
{
}
