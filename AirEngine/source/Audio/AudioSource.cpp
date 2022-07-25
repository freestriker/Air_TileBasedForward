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
	: Component(ComponentType::AUDIO_SOURCE)
    , _audioClip(nullptr)
	, _loop(false)
	, _pitch(1)
	, _gain(1)
    , _source()
    , _dirty(false)
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

ALuint AirEngine::Audio::AudioSource::Source()
{
    return _source;
}

float AirEngine::Audio::AudioSource::GetTime()
{
    float curTime = 0;
    float* curTimePtr = &curTime;
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, curTimePtr]()->void
        {
            Utils::Log::Exception("Previous error in get time.", alGetError());
            alGetSourcef(this->_source, AL_SEC_OFFSET, curTimePtr);
            Utils::Log::Exception("Failed to get time.", alGetError());
        }
    );
    return curTime;
}

void AirEngine::Audio::AudioSource::SetTime(float time)
{
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, time]()->void
        {
            Utils::Log::Exception("Previous error in set time.", alGetError());
            alSourcef(this->_source, AL_SEC_OFFSET, time);
            Utils::Log::Exception("Failed to set time.", alGetError());
        }
    );
}

ALenum AirEngine::Audio::AudioSource::State()
{
    ALenum state = 0;
    ALenum* statePtr = &state;
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, statePtr]()->void
        {
            Utils::Log::Exception("Previous error in get state.", alGetError());
            alGetSourcei(this->_source, AL_SOURCE_STATE, statePtr);
            Utils::Log::Exception("Failed to get state.", alGetError());
        }
    );
    return state;
}

void AirEngine::Audio::AudioSource::Play(Asset::AudioClip* audioClip)
{
    this->_audioClip = audioClip;
    Stop();
    Play();
}

void AirEngine::Audio::AudioSource::Play()
{
    glm::vec3 pos = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, pos]()->void
        {
            Utils::Log::Exception("Previous error in updata audio source data.", alGetError());
            alSourcei(this->_source, AL_BUFFER, this->_audioClip->Buffer());
            alSourcef(this->_source, AL_PITCH, this->_pitch);
            alSourcef(this->_source, AL_GAIN, this->_gain);
            alSourcefv(this->_source, AL_POSITION, reinterpret_cast<const ALfloat*>(&pos));
            alSourcei(this->_source, AL_LOOPING, this->_gain ? 1 : 0);
            Utils::Log::Exception("Failed to update audio source.", alGetError());
            alSourcePlay(this->_source);
            Utils::Log::Exception("Failed to play audio source.", alGetError());
        }
    );

    _dirty = false;
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

AirEngine::Asset::AudioClip* AirEngine::Audio::AudioSource::GetAudioClip()
{
    return _audioClip;
}

void AirEngine::Audio::AudioSource::SetAudioClip(Asset::AudioClip* audioClip)
{
    this->_audioClip = audioClip;
    Stop();
}

bool AirEngine::Audio::AudioSource::GetLoop()
{
    return _loop;
}

void AirEngine::Audio::AudioSource::SetLoop(bool loop)
{
    _loop = loop;
    _dirty = true;
}

float AirEngine::Audio::AudioSource::GetPitch()
{
    return _pitch;
}

void AirEngine::Audio::AudioSource::SetPitch(float pitch)
{
    _pitch = pitch;
    _dirty = true;
}

float AirEngine::Audio::AudioSource::GetGain()
{
    return _gain;
}

void AirEngine::Audio::AudioSource::SetGain(float gain)
{
    _gain = gain;
    _dirty = true;
}

void AirEngine::Audio::AudioSource::OnAwake()
{
}

void AirEngine::Audio::AudioSource::OnStart()
{
}

void AirEngine::Audio::AudioSource::OnUpdate()
{
    if (_audioClip == nullptr) return;

    glm::vec3 pos = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, pos]()->void
        {
            Utils::Log::Exception("Previous error in updata audio source data.", alGetError());
            if (this->_dirty)
            {
                alSourcef(this->_source, AL_PITCH, this->_pitch);
                alSourcef(this->_source, AL_GAIN, this->_gain);
                alSourcei(this->_source, AL_LOOPING, this->_gain ? 1 : 0);
            }
            alSourcefv(this->_source, AL_POSITION, reinterpret_cast<const ALfloat*>(&pos));
            Utils::Log::Exception("Failed to update audio source.", alGetError());
        }
    );

    _dirty = false;
}

void AirEngine::Audio::AudioSource::OnDestroy()
{
}
