#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class DirectionalLight final : public AirEngine::Light::LightBase
		{
		public:
			LightInfo GetLightInfo() override;
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