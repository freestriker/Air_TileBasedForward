#pragma once
#include "Core/Logic/Object/Component.h"

namespace AirEngine
{
	namespace Audio
	{
		class AudioListener final : public Core::Logic::Object::Component
		{
		private:
			static bool _isCreated;
			float _gain;
			virtual void OnAwake()override;
			virtual void OnStart()override;
			virtual void OnUpdate()override;
			virtual void OnDestroy()override;
		public:
			AudioListener();
			virtual ~AudioListener();

			float GetGain();
			void SetGain(float gain);

			AudioListener(const AudioListener&) = delete;
			AudioListener& operator=(const AudioListener&) = delete;
			AudioListener(AudioListener&&) = delete;
			AudioListener& operator=(AudioListener&&) = delete;

			RTTR_ENABLE(Core::Logic::Object::Component)
		};
	}
}