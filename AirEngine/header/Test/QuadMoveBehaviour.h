#pragma once
#include "Behaviour/Behaviour.h"
#include <string>

namespace AirEngine
{
	namespace Test
	{
		class QuadMoveBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			QuadMoveBehaviour();
			~QuadMoveBehaviour();
			QuadMoveBehaviour(const QuadMoveBehaviour&) = delete;
			QuadMoveBehaviour& operator=(const QuadMoveBehaviour&) = delete;
			QuadMoveBehaviour(QuadMoveBehaviour&&) = delete;
			QuadMoveBehaviour& operator=(QuadMoveBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;
			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
