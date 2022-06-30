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
		class MirrorRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::future<Asset::Mesh*> meshTask;
			std::future<Core::Graphic::Shader*> shaderTask;
			bool loaded;
			Asset::Mesh* mesh;
			Core::Graphic::Shader* shader;
			Core::Graphic::Material* material;
		public:
			MirrorRendererBehaviour();
			~MirrorRendererBehaviour();
			MirrorRendererBehaviour(const MirrorRendererBehaviour&) = delete;
			MirrorRendererBehaviour& operator=(const MirrorRendererBehaviour&) = delete;
			MirrorRendererBehaviour(MirrorRendererBehaviour&&) = delete;
			MirrorRendererBehaviour& operator=(MirrorRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
