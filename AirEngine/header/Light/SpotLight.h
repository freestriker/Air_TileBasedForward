#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class SpotLight final : public AirEngine::Light::LightBase
		{
		public:
			float minRange;
			float maxRange;
			float innerAngle;
			float outerAngle;
			LightInfo GetLightInfo() override;
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