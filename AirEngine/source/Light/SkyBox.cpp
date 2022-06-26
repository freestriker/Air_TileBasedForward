#include "Light/SkyBox.h"

AirEngine::Light::LightBase::LightData AirEngine::Light::SkyBox::GetLightData()
{
	LightData lightDate{};
	lightDate.type = 3;
	lightDate.intensity = intensity;
	lightDate.range = 0;
	lightDate.extraParamter = glm::vec4(0);
	lightDate.position = glm::vec3(0);
	lightDate.color = color;
	return lightDate;
}

AirEngine::Light::SkyBox::SkyBox()
	: LightBase(LightType::SKY_BOX)
	, skyBoxTextureCube(nullptr)
{
}

AirEngine::Light::SkyBox::~SkyBox()
{
}
