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
		class TransparentRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::future<Asset::Mesh*> meshTask;
			std::future<Core::Graphic::Shader*> shaderTask;
			std::future<Asset::Texture2D*> diffuseTexture2DTask;
			bool loaded;
			Asset::Mesh* mesh;
			Core::Graphic::Shader* shader;
			Asset::Texture2D* diffuseTexture2D;
			Core::Graphic::Material* material;
		public:
			TransparentRendererBehaviour();
			~TransparentRendererBehaviour();
			TransparentRendererBehaviour(const TransparentRendererBehaviour&) = delete;
			TransparentRendererBehaviour& operator=(const TransparentRendererBehaviour&) = delete;
			TransparentRendererBehaviour(TransparentRendererBehaviour&&) = delete;
			TransparentRendererBehaviour& operator=(TransparentRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
