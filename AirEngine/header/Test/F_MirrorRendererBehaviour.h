#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Asset/Texture2D.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>

namespace AirEngine
{
	namespace Test
	{
		class F_MirrorRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			F_MirrorRendererBehaviour();
			~F_MirrorRendererBehaviour();
			F_MirrorRendererBehaviour(const F_MirrorRendererBehaviour&) = delete;
			F_MirrorRendererBehaviour& operator=(const F_MirrorRendererBehaviour&) = delete;
			F_MirrorRendererBehaviour(F_MirrorRendererBehaviour&&) = delete;
			F_MirrorRendererBehaviour& operator=(F_MirrorRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
