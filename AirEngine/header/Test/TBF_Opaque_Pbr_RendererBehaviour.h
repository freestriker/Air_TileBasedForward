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
		class TBF_Opaque_Pbr_RendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::string _meshPath;
			std::string _textureBasePath;
		public:
			TBF_Opaque_Pbr_RendererBehaviour(std::string meshPath, std::string textureBasePath);
			CONSTRUCTOR(TBF_Opaque_Pbr_RendererBehaviour)
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
