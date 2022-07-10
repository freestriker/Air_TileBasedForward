#include "Light/LightBase.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Light::LightBase>("AirEngine::Light::LightBase");
}

const AirEngine::Light::LightBase::LightInfo* AirEngine::Light::LightBase::GetLightInfo()
{
	return &_lightInfo;
}

const std::array<glm::vec4, 8>* AirEngine::Light::LightBase::GetBoundingBox()
{
	return &_boundingBoxVertexes;
}

void AirEngine::Light::LightBase::OnUpdate()
{
	OnSetLightInfo(_lightInfo);
	OnSetBoundingBoxInfo(_boundingBoxVertexes);
}

AirEngine::Light::LightBase::LightBase(LightType lightType)
	: Component(Component::ComponentType::LIGHT)
	, lightType(lightType)
	, color({ 1, 1, 1, 1 })
	, intensity(1.0f)
	, _lightInfo()
	, _boundingBoxVertexes()
{
}

AirEngine::Light::LightBase::~LightBase()
{
}
