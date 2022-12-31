#pragma once
#include <glm/glm.hpp>
#include "Light/LightBase.h"

namespace AirEngine
{
	namespace Core::Graphic::Instance
	{
		class Image;
	}
	namespace Light
	{
		class AmbientLight final : public AirEngine::Light::LightBase
		{
		private:
			void OnSetLightInfo(LightInfo& info)override;
			void OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)override;
		public:
			Core::Graphic::Instance::Image* _irradianceCubeImage;
			Core::Graphic::Instance::Image* _prefilteredCubeImage;
			Core::Graphic::Instance::Image* _lutImage;

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