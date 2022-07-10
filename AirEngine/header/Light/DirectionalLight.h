#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class DirectionalLight final : public AirEngine::Light::LightBase
		{
		private:
			void OnSetLightInfo(LightInfo& info)override;
			void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)override;
		public:
			DirectionalLight();
			~DirectionalLight();
			DirectionalLight(const DirectionalLight&) = delete;
			DirectionalLight& operator=(const DirectionalLight&) = delete;
			DirectionalLight(DirectionalLight&&) = delete;
			DirectionalLight& operator=(DirectionalLight&&) = delete;

			RTTR_ENABLE(AirEngine::Light::LightBase)
		};
	}
}