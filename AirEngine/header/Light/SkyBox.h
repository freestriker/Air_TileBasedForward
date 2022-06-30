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
		class SkyBox final : public AirEngine::Light::LightBase
		{
		public:
			Asset::TextureCube* skyBoxTextureCube;
			LightData GetLightData() override;
			SkyBox();
			~SkyBox();
			SkyBox(const SkyBox&) = delete;
			SkyBox& operator=(const SkyBox&) = delete;
			SkyBox(SkyBox&&) = delete;
			SkyBox& operator=(SkyBox&&) = delete;

			RTTR_ENABLE(AirEngine::Light::LightBase)
		};
	}
}