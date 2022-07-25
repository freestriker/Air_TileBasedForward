#pragma once
#include "Behaviour/Behaviour.h"
#include <string>

namespace AirEngine
{
	namespace Test
	{
		class AudioSourceBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::string _audioClipPath;
		public:
			AudioSourceBehaviour(std::string audioClipPath);
			~AudioSourceBehaviour();
			AudioSourceBehaviour(const AudioSourceBehaviour&) = delete;
			AudioSourceBehaviour& operator=(const AudioSourceBehaviour&) = delete;
			AudioSourceBehaviour(AudioSourceBehaviour&&) = delete;
			AudioSourceBehaviour& operator=(AudioSourceBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;
			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
