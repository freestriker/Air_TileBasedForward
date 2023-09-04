#pragma once
#include "Behaviour/Behaviour.h"

namespace AirEngine
{
	namespace Test
	{
		class IfftOcean_WindRotationBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			float windRotationAngleSpeed;
			CONSTRUCTOR(IfftOcean_WindRotationBehaviour)
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
