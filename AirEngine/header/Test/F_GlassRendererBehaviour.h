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
		class F_GlassRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		public:
			CONSTRUCTOR(F_GlassRendererBehaviour)
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
