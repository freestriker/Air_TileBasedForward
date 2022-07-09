#include "Light/AmbientLight.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::AmbientLight>("AirEngine::Light::AmbientLight");
}

AirEngine::Light::LightBase::LightInfo AirEngine::Light::AmbientLight::GetLightInfo()
{
	LightInfo lightDate{};
	lightDate.type = 3;
	lightDate.intensity = intensity;
	lightDate.minRange = 0;
	lightDate.maxRange = 0;
	lightDate.extraParamter = glm::vec4(0);
	lightDate.position = glm::vec3(0);
	lightDate.direction = { 0, 0, 0 };
	lightDate.color = color;
	return lightDate;
}

AirEngine::Light::AmbientLight::AmbientLight()
	: LightBase(LightType::AMBIENT)
	, ambientLightTextureCube(nullptr)
{
}

AirEngine::Light::AmbientLight::~AmbientLight()
{
}
