#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"
#include "Core/Graphic/Shader.h"
#include "Asset/Texture2D.h"
#include "Core/Graphic/Material.h"
#include <future>

namespace AirEngine
{
	namespace Test
	{
		class TBF_OIT_RedBoxBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			TBF_OIT_RedBoxBehaviour();
			~TBF_OIT_RedBoxBehaviour();
			TBF_OIT_RedBoxBehaviour(const TBF_OIT_RedBoxBehaviour&) = delete;
			TBF_OIT_RedBoxBehaviour& operator=(const TBF_OIT_RedBoxBehaviour&) = delete;
			TBF_OIT_RedBoxBehaviour(TBF_OIT_RedBoxBehaviour&&) = delete;
			TBF_OIT_RedBoxBehaviour& operator=(TBF_OIT_RedBoxBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
