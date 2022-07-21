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
			ALuint _source;
		public:
			Asset::AudioClip* audioClip;
			bool loop;
			float pitch;
			float gain;
			AudioSource();
			virtual ~AudioSource();

			void Play(Asset::AudioClip* audioClip);
			void Play();
			void Pause();
			void Stop();

			virtual void OnAwake()override;
			virtual void OnStart()override;
			virtual void OnUpdate()override;
			virtual void OnDestroy()override;
			AudioSource(const AudioSource&) = delete;
			AudioSource& operator=(const AudioSource&) = delete;
			AudioSource(AudioSource&&) = delete;
			AudioSource& operator=(AudioSource&&) = delete;
		};
	}
}