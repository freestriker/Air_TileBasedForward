#include "Behaviour/Behaviour.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Behaviour::Behaviour>("AirEngine::Behaviour::Behaviour");
}

AirEngine::Behaviour::Behaviour::Behaviour()
	: Component(ComponentType::BEHAVIOUR)
{
}

AirEngine::Behaviour::Behaviour::~Behaviour()
{
}

void AirEngine::Behaviour::Behaviour::OnAwake()
{
}

void AirEngine::Behaviour::Behaviour::OnStart()
{
}

void AirEngine::Behaviour::Behaviour::OnUpdate()
{
}

void AirEngine::Behaviour::Behaviour::OnDestroy()
{
}
