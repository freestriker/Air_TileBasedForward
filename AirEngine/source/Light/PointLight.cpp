#include "Light/PointLight.h"
#include "Core/Logic/Object/GameObject.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::PointLight>("AirEngine::Light::PointLight");
}

AirEngine::Light::LightBase::LightData AirEngine::Light::PointLight::GetLightData()
{
	LightData lightDate{};
	lightDate.type = 2;
	lightDate.intensity = intensity;
	lightDate.minRange = minRange;
	lightDate.minRange = maxRange;
	lightDate.extraParamter = { 0, 0, 0, 0 };
	lightDate.position = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);
	lightDate.direction = { 0, 0, 0 };
	lightDate.color = color;
	return lightDate;
}

AirEngine::Light::PointLight::PointLight()
	: LightBase(LightBase::LightType::POINT)
	, minRange(1.0f)
	, maxRange(10.0f)
{
}

AirEngine::Light::PointLight::~PointLight()
{
}
