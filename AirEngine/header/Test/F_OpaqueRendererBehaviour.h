#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>

namespace AirEngine
{
	namespace Test
	{
		class F_OpaqueRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			float _rotationSpeed;
		public:
			F_OpaqueRendererBehaviour();
			~F_OpaqueRendererBehaviour();
			F_OpaqueRendererBehaviour(const F_OpaqueRendererBehaviour&) = delete;
			F_OpaqueRendererBehaviour& operator=(const F_OpaqueRendererBehaviour&) = delete;
			F_OpaqueRendererBehaviour(F_OpaqueRendererBehaviour&&) = delete;
			F_OpaqueRendererBehaviour& operator=(F_OpaqueRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
