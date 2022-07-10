#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class SpotLight final : public AirEngine::Light::LightBase
		{
		private:
			void OnSetLightInfo(LightInfo& info)override;
			void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)override;
		public:
			float minRange;
			float maxRange;
			float innerAngle;
			float outerAngle;
			SpotLight();
			~SpotLight();
			SpotLight(const SpotLight&) = delete;
			SpotLight& operator=(const SpotLight&) = delete;
			SpotLight(SpotLight&&) = delete;
			SpotLight& operator=(SpotLight&&) = delete;

			RTTR_ENABLE(AirEngine::Light::LightBase)
		};
	}
}