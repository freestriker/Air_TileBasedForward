#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Asset
	{
		class TextureCube;
	}
	namespace Light
	{
		class AmbientLight final : public AirEngine::Light::LightBase
		{
		private:
			void OnSetLightInfo(LightInfo& info)override;
			void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)override;
		public:
			Asset::TextureCube* ambientLightTextureCube;

			AmbientLight();
			~AmbientLight();
			AmbientLight(const AmbientLight&) = delete;
			AmbientLight& operator=(const AmbientLight&) = delete;
			AmbientLight(AmbientLight&&) = delete;
			AmbientLight& operator=(AmbientLight&&) = delete;

			RTTR_ENABLE(AirEngine::Light::LightBase)
		};
	}
}