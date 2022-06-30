#include "Light/SpotLight.h"
#include "Core/Logic/Object/GameObject.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::SpotLight>("AirEngine::Light::SpotLight");
}

AirEngine::Light::LightBase::LightData AirEngine::Light::SpotLight::GetLightData()
{
	LightData lightDate{};
	lightDate.type = 4;
	lightDate.intensity = intensity;
	lightDate.minRange = minRange;
	lightDate.maxRange = maxRange;
	lightDate.extraParamter = { innerAngle, outerAngle, 0, 0 };
	lightDate.position = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);
	lightDate.direction = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 0)));
	lightDate.color = color;
	return lightDate;
}

AirEngine::Light::SpotLight::SpotLight()
	: LightBase(LightBase::LightType::SPOT)
	, minRange(0.01f)
	, maxRange(10.0f)
	, innerAngle(30.0f)
	, outerAngle(60.0f)
{
}

AirEngine::Light::SpotLight::~SpotLight()
{
}
