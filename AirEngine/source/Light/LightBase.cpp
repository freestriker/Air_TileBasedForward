#include "Light/LightBase.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::LightBase>("AirEngine::Light::LightBase");
}

AirEngine::Light::LightBase::LightBase(LightType lightType)
	: Component(Component::ComponentType::LIGHT)
	, lightType(lightType)
	, color({ 1, 1, 1, 1 })
	, intensity(1.0f)
{
}

AirEngine::Light::LightBase::~LightBase()
{
}
