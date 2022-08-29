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
		class F_GlassRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			F_GlassRendererBehaviour();
			~F_GlassRendererBehaviour();
			F_GlassRendererBehaviour(const F_GlassRendererBehaviour&) = delete;
			F_GlassRendererBehaviour& operator=(const F_GlassRendererBehaviour&) = delete;
			F_GlassRendererBehaviour(F_GlassRendererBehaviour&&) = delete;
			F_GlassRendererBehaviour& operator=(F_GlassRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
