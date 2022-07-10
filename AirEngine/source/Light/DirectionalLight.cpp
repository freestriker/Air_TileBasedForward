#include "Light/DirectionalLight.h"
#include "Core/Logic/Object/GameObject.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::DirectionalLight>("AirEngine::Light::DirectionalLight");
}

void AirEngine::Light::DirectionalLight::OnSetLightInfo(LightInfo& info)
{
	info.type = 1;
	info.intensity = intensity;
	info.minRange = 0;
	info.maxRange = 0;
	info.extraParamter = { 0, 0, 0, 0 };
	info.position = glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1));
	info.direction = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 0)));
	info.color = color;
}

void AirEngine::Light::DirectionalLight::OnSetBoundingBoxInfo(std::array<glm::vec4, 8>& boundingBoxVertexes)
{
}

AirEngine::Light::DirectionalLight::DirectionalLight()
	: LightBase(LightBase::LightType::DIRECTIONAL)
{
}

AirEngine::Light::DirectionalLight::~DirectionalLight()
{
}
