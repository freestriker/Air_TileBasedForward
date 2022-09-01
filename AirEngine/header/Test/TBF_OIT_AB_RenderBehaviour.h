#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Asset/Texture2D.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>

namespace AirEngine
{
	namespace Test
	{
		class TBF_OIT_AB_RenderBehaviour : public AirEngine::Behaviour::Behaviour
		{
			std::string _diffuseTextureName;
		public:
			TBF_OIT_AB_RenderBehaviour(std::string diffuseTextureName);
			CONSTRUCTOR(TBF_OIT_AB_RenderBehaviour)
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
