#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Light
	{
		class PointLight final : public AirEngine::Light::LightBase
		{
		private:
			void OnSetLightInfo(LightInfo& info)override;
			void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)override;
		public:
			float minRange;
			float maxRange;
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