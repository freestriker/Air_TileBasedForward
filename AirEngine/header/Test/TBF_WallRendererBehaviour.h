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
		class TBF_WallRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::string _meshPath;
		public:
			TBF_WallRendererBehaviour(std::string mesh);
			CONSTRUCTOR(TBF_WallRendererBehaviour)
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
