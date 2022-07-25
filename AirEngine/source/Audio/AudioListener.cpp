#include "Audio/AudioListener.h"
#include "Utils/Log.h"
#include "Core/Audio/CoreObject/Instance.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"

bool AirEngine::Audio::AudioListener::_isCreated = false;

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Audio::AudioListener>("AirEngine::Audio::AudioListener");
}

void AirEngine::Audio::AudioListener::OnAwake()
{
}

void AirEngine::Audio::AudioListener::OnStart()
{
}

void AirEngine::Audio::AudioListener::OnUpdate()
{
    glm::vec3 pos = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, pos]()->void
        {
            Utils::Log::Exception("Previous error in update audio listener.", alGetError());
            alListenerf(AL_GAIN, this->_gain);
            alListenerfv(AL_POSITION, reinterpret_cast<const ALfloat*>(&pos));
            Utils::Log::Exception("Failed to update audio listener.", alGetError());
        }
    );
}

void AirEngine::Audio::AudioListener::OnDestroy()
{
}

AirEngine::Audio::AudioListener::AudioListener()
	: Component(ComponentType::AUDIO_LISTENER)
    , _gain(1.0f)
{
    Utils::Log::Exception("Already created a audio listener.", _isCreated);
}

AirEngine::Audio::AudioListener::~AudioListener()
{
}

float AirEngine::Audio::AudioListener::GetGain()
{
	return _gain;
}

void AirEngine::Audio::AudioListener::SetGain(float gain)
{
	_gain = gain;
}
