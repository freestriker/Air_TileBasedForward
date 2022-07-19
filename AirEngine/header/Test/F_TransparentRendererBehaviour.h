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
		class F_TransparentRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			F_TransparentRendererBehaviour();
			~F_TransparentRendererBehaviour();
			F_TransparentRendererBehaviour(const F_TransparentRendererBehaviour&) = delete;
			F_TransparentRendererBehaviour& operator=(const F_TransparentRendererBehaviour&) = delete;
			F_TransparentRendererBehaviour(F_TransparentRendererBehaviour&&) = delete;
			F_TransparentRendererBehaviour& operator=(F_TransparentRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
