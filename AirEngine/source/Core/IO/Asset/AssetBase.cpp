#include "Core/IO/Asset/AssetBase.h"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Core::IO::Asset::AssetBase>("AirEngine::Core::IO::Asset::AssetBase");
}

AirEngine::Core::IO::Asset::AssetBase::AssetBase(bool isLoaded)
	: _isHeldByManager(isLoaded)
{
}

AirEngine::Core::IO::Asset::AssetBase::~AssetBase()
{
}

bool AirEngine::Core::IO::Asset::AssetBase::IsHeldByManager()
{
	return _isHeldByManager;
}

std::string AirEngine::Core::IO::Asset::AssetBase::Path()
{
	return _path;
}
