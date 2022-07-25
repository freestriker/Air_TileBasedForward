#pragma once
#include "Core/Logic/Object/Component.h"
#include "AL/al.h"

namespace AirEngine
{
	namespace Asset
	{
		class AudioClip;
	}
	namespace Audio
	{
		class AudioSource : public Core::Logic::Object::Component
		{
		private:
			Asset::AudioClip* _audioClip;
			ALuint _source;
			bool _dirty;
			bool _loop;
			float _pitch;
			float _gain;

			virtual void OnAwake()override;
			virtual void OnStart()override;
			virtual void OnUpdate()override;
			virtual void OnDestroy()override;
		public:
			AudioSource();
			virtual ~AudioSource();
			ALuint Source();
			ALenum State();

			void Play(Asset::AudioClip* audioClip);
			void Play();
			void Pause();
			void Stop();

			float GetTime();
			void SetTime(float time);
			Asset::AudioClip* GetAudioClip();
			void SetAudioClip(Asset::AudioClip* audioClip);
			bool GetLoop();
			void SetLoop(bool loop);
			float GetPitch();
			void SetPitch(float pitch);
			float GetGain();
			void SetGain(float gain);


			AudioSource(const AudioSource&) = delete;
			AudioSource& operator=(const AudioSource&) = delete;
			AudioSource(AudioSource&&) = delete;
			AudioSource& operator=(AudioSource&&) = delete;

			RTTR_ENABLE(Core::Logic::Object::Component)
		};
	}
}