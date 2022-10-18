#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"

namespace AirEngine
{
	namespace Test
	{
		class CameraMoveBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:

			float _rotation;

			CameraMoveBehaviour();
			~CameraMoveBehaviour();
			CameraMoveBehaviour(const CameraMoveBehaviour&) = delete;
			CameraMoveBehaviour& operator=(const CameraMoveBehaviour&) = delete;
			CameraMoveBehaviour(CameraMoveBehaviour&&) = delete;
			CameraMoveBehaviour& operator=(CameraMoveBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;
			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
