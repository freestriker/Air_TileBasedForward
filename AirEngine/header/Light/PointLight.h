#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class PointLight final : public AirEngine::Light::LightBase
		{
		public:
			float minRange;
			float maxRange;
			LightInfo GetLightInfo() override;
			PointLight();
			~PointLight();
			PointLight(const PointLight&) = delete;
			PointLight& operator=(const PointLight&) = delete;
			PointLight(PointLight&&) = delete;
			PointLight& operator=(PointLight&&) = delete;

			RTTR_ENABLE(AirEngine::Light::LightBase)
		};
	}
}