#pragma once
#include "Behaviour/Behaviour.h"
#include <string>
#include "Asset/Mesh.h"
#include "Core/Graphic/Shader.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Material.h"
#include <future>

namespace AirEngine
{
	namespace Test
	{
		class BackgroundRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::future<Asset::Mesh*> meshTask;
			std::future<Core::Graphic::Shader*> shaderTask;
			std::future<Asset::TextureCube*> textureCubeTask;
			bool loaded;
			Asset::Mesh* mesh;
			Core::Graphic::Shader* shader;
			Asset::TextureCube* textureCube;
			Core::Graphic::Material* material;
		public:
			BackgroundRendererBehaviour();
			~BackgroundRendererBehaviour();
			BackgroundRendererBehaviour(const BackgroundRendererBehaviour&) = delete;
			BackgroundRendererBehaviour& operator=(const BackgroundRendererBehaviour&) = delete;
			BackgroundRendererBehaviour(BackgroundRendererBehaviour&&) = delete;
			BackgroundRendererBehaviour& operator=(BackgroundRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
