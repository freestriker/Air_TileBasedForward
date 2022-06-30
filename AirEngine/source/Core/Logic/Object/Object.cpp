#include "Core/Logic/Object/Object.h"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::Logic::Object::Object>("AirEngine::Core::Logic::Object::Object");
}

AirEngine::Core::Logic::Object::Object::Object()
{
}

AirEngine::Core::Logic::Object::Object::~Object()
{
}

rttr::type AirEngine::Core::Logic::Object::Object::Type()
{
	return rttr::type::get(*this);
}

std::string AirEngine::Core::Logic::Object::Object::ToString()
{
	return Type().get_name().to_string();
}

void AirEngine::Core::Logic::Object::Object::OnDestroy()
{
}

void AirEngine::Core::Logic::Object::Object::OnAwake()
{
}
