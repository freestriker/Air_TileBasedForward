#include "Core/IO/Asset/AssetBase.h"

AirEngine::Core::IO::Asset::AssetBsse::AssetBsse(bool isLoaded)
	: _isHeldByManager(isLoaded)
{
}

AirEngine::Core::IO::Asset::AssetBsse::~AssetBsse()
{
}

bool AirEngine::Core::IO::Asset::AssetBsse::IsHeldByManager()
{
	return _isHeldByManager;
}

std::string AirEngine::Core::IO::Asset::AssetBsse::Path()
{
	return _path;
}
