#pragma once
#include <glm/glm.hpp>
#include "Core/Logic/Object/Component.h"
#include <array>
#include "glm/glm.hpp"

namespace AirEngine
{
	namespace Light
	{
		class LightBase : public AirEngine::Core::Logic::Object::Component
		{
		public:
			struct LightInfo
			{
				alignas(4) int type;
				alignas(4) float intensity;
				alignas(4) float minRange;
				alignas(4) float maxRange;
				alignas(16) glm::vec4 extraParamter;
				alignas(16) glm::vec3 position;
				alignas(16) glm::vec3 direction;
				alignas(16) glm::vec4 color;
			};
			enum class LightType
			{
				DIRECTIONAL = 1,
				POINT = 2,
				AMBIENT = 3,
				SPOT = 4
			};
			const LightInfo* GetLightInfo();
			const std::array<glm::vec4, 8>* GetBoundingBox();
			glm::vec4 color;
			float intensity;
			const LightType lightType;
		private:
			LightInfo _lightInfo;
			std::array<glm::vec4, 8> _boundingBoxVertexes;
			void OnUpdate()override;
		protected:
			LightBase(LightType lightType);
			~LightBase();
			LightBase(const LightBase&) = delete;
			LightBase& operator=(const LightBase&) = delete;
			LightBase(LightBase&&) = delete;
			LightBase& operator=(LightBase&&) = delete;

			virtual void OnSetLightInfo(LightInfo& info) = 0;
			virtual void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes) = 0;

			RTTR_ENABLE(AirEngine::Core::Logic::Object::Component)
		};
	}
}