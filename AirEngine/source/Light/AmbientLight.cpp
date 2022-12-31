#include "Light/AmbientLight.h"
#include "Core/Graphic/Instance/Image.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::AmbientLight>("AirEngine::Light::AmbientLight");
}

void AirEngine::Light::AmbientLight::OnSetLightInfo(LightInfo& info)
{
	info.type = 3;
	info.intensity = intensity;
	info.minRange = 0;
	info.maxRange = 0;
	info.extraParamter = glm::vec4(_prefilteredCubeImage->ImageView_().MipmapLevelCount() - 1, 0, 0, 0);
	info.position = glm::vec3(0);
	info.direction = { 0, 0, 0 };
	info.color = color;
}

void AirEngine::Light::AmbientLight::OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)
{

}

AirEngine::Light::AmbientLight::AmbientLight()
	: LightBase(LightType::AMBIENT)
	, _irradianceCubeImage(nullptr)
	, _prefilteredCubeImage(nullptr)
	, _lutImage(nullptr)
{
}

AirEngine::Light::AmbientLight::~AmbientLight()
{
}
