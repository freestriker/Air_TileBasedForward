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
		class TBF_OIT_RenderBehaviour : public AirEngine::Behaviour::Behaviour
		{
			std::string _diffuseTextureName;
		public:
			TBF_OIT_RenderBehaviour(std::string diffuseTextureName);
			TBF_OIT_RenderBehaviour();
			~TBF_OIT_RenderBehaviour();
			TBF_OIT_RenderBehaviour(const TBF_OIT_RenderBehaviour&) = delete;
			TBF_OIT_RenderBehaviour& operator=(const TBF_OIT_RenderBehaviour&) = delete;
			TBF_OIT_RenderBehaviour(TBF_OIT_RenderBehaviour&&) = delete;
			TBF_OIT_RenderBehaviour& operator=(TBF_OIT_RenderBehaviour&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;

			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
