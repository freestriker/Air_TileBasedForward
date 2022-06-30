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
		class OpaqueRendererBehaviour : public AirEngine::Behaviour::Behaviour
		{
		private:
			std::future<Asset::Mesh*> meshTask;
			std::future<Core::Graphic::Shader*> shaderTask;
			std::future<Asset::Texture2D*> diffuseTexture2DTask;
			std::future<Asset::Texture2D*> normalTexture2DTask;
			bool loaded;
			Asset::Mesh* mesh;
			Core::Graphic::Shader* shader;
			Asset::Texture2D* diffuseTexture2D;
			Asset::Texture2D* normalTexture2D;
			Core::Graphic::Material* material;
			float rotationSpeed;
		public:
			OpaqueRendererBehaviour();
			~OpaqueRendererBehaviour();
			OpaqueRendererBehaviour(const OpaqueRendererBehaviour&) = delete;
			OpaqueRendererBehaviour& operator=(const OpaqueRendererBehaviour&) = delete;
			OpaqueRendererBehaviour(OpaqueRendererBehaviour&&) = delete;
			OpaqueRendererBehaviour& operator=(OpaqueRendererBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
