#include "Light/DirectionalLight.h"
#include "Core/Logic/Object/GameObject.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::DirectionalLight>("AirEngine::Light::DirectionalLight");
}

AirEngine::Light::LightBase::LightInfo AirEngine::Light::DirectionalLight::GetLightInfo()
{
	LightInfo lightDate{};
	lightDate.type = 1;
	lightDate.intensity = intensity;
	lightDate.minRange = 0;
	lightDate.maxRange = 0;
	lightDate.extraParamter = { 0, 0, 0, 0 };
	lightDate.position = glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1));
	lightDate.direction = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 0)));
	lightDate.color = color;
	return lightDate;
}

AirEngine::Light::DirectionalLight::DirectionalLight()
	: LightBase(LightBase::LightType::DIRECTIONAL)
{
}

AirEngine::Light::DirectionalLight::~DirectionalLight()
{
}
