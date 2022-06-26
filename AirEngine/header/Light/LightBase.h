#pragma once
#include <glm/glm.hpp>
#include "Core/Logic/Object/Component.h"

namespace AirEngine
{
	namespace Light
	{
		class LightBase : public AirEngine::Core::Logic::Object::Component
		{
		public:
			struct LightData
			{
				alignas(4) int type;
				alignas(4) float intensity;
				alignas(4) float range;
				alignas(16) glm::vec4 extraParamter;
				alignas(16) glm::vec3 position;
				alignas(16) glm::vec4 color;
			};
			enum class LightType
			{
				DIRECTIONAL = 1,
				POINT = 2,
				SKY_BOX = 3
			};
			virtual LightData GetLightData() = 0;
			glm::vec4 color;
			float intensity;
			const LightType lightType;
		protected:
			LightBase(LightType lightType);
			~LightBase();
			LightBase(const LightBase&) = delete;
			LightBase& operator=(const LightBase&) = delete;
			LightBase(LightBase&&) = delete;
			LightBase& operator=(LightBase&&) = delete;

			RTTR_ENABLE(AirEngine::Core::Logic::Object::Component)
		};
	}
}