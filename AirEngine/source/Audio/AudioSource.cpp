#include "Audio/AudioSource.h"
#include "Asset/AudioClip.h"
#include "Core/Audio/CoreObject/Instance.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Utils/Log.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Audio::AudioSource>("AirEngine::Audio::AudioSource");
}

AirEngine::Audio::AudioSource::AudioSource()
	: audioClip(nullptr)
	, loop(false)
	, pitch(1)
	, gain(1)
    , _source()
{
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this]()->void
        {
            Utils::Log::Exception("Previous error in create audio source.", alGetError());
            alGenSources(1, &this->_source);
            Utils::Log::Exception("Failed to create audio source.", alGetError());
        }
    );
}

AirEngine::Audio::AudioSource::~AudioSource()
{
}

void AirEngine::Audio::AudioSource::Play(Asset::AudioClip* audioClip)
{
    this->audioClip = audioClip;
    Play();
}

void AirEngine::Audio::AudioSource::Play()
{
    glm::vec3 pos = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, pos]()->void
        {
            Utils::Log::Exception("Previous error in updata audio source data.", alGetError());
            alSourcei(this->_source, AL_BUFFER, 0);
            alSourcei(this->_source, AL_BUFFER, this->audioClip->AlBuffer());
            alSourcef(this->_source, AL_PITCH, this->pitch);
            alSourcef(this->_source, AL_GAIN, this->gain);
            alSourcefv(this->_source, AL_POSITION, reinterpret_cast<const ALfloat*>(&pos));
            alSourcei(this->_source, AL_LOOPING, this->gain ? 1 : 0);
            Utils::Log::Exception("Failed to update audio source.", alGetError());
            alSourcePlay(this->_source);
            Utils::Log::Exception("Failed to play audio source.", alGetError());
        }
    );
}

void AirEngine::Audio::AudioSource::Pause()
{
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this]()->void
        {
            Utils::Log::Exception("Previous error in pause audio source.", alGetError());
            alSourcePause(this->_source);
            Utils::Log::Exception("Failed to pause audio source.", alGetError());
        }
    );
}

void AirEngine::Audio::AudioSource::Stop()
{
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this]()->void
        {
            Utils::Log::Exception("Previous error in stop audio source.", alGetError());
            alSourceStop(this->_source);
            Utils::Log::Exception("Failed to stop audio source.", alGetError());
        }
    );
}

void AirEngine::Audio::AudioSource::OnAwake()
{
}

void AirEngine::Audio::AudioSource::OnStart()
{
}

void AirEngine::Audio::AudioSource::OnUpdate()
{
    glm::vec3 pos = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, pos]()->void
        {
            Utils::Log::Exception("Previous error in updata audio source data.", alGetError());
            alSourcef(this->_source, AL_PITCH, this->pitch);
            alSourcef(this->_source, AL_GAIN, this->gain);
            alSourcefv(this->_source, AL_POSITION, reinterpret_cast<const ALfloat*>(&pos));
            alSourcei(this->_source, AL_LOOPING, this->gain ? 1 : 0);
            Utils::Log::Exception("Failed to update audio source.", alGetError());
        }
    );
}

void AirEngine::Audio::AudioSource::OnDestroy()
{
}
