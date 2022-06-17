#include "Core/IO/Manager/AssetManager.h"

AirEngine::Core::IO::Manager::AssetManager::AssetManager()
	: _wrappers()
	, _mutex()
{
}

AirEngine::Core::IO::Manager::AssetManager::~AssetManager()
{
}

void AirEngine::Core::IO::Manager::AssetManager::Unload(Asset::AssetBase*& asset)
{
	std::unique_lock<std::mutex> wrapperLock(asset->_wrapper->mutex);

	asset->_wrapper->referenceCount--;
	asset = nullptr;
}

void AirEngine::Core::IO::Manager::AssetManager::Unload(std::string path)
{
	std::unique_lock<std::mutex> managerLock(_mutex);

	//Get target pool
	AssetWrapper* targetWrapper = _wrappers[path];

	{
		std::unique_lock<std::mutex> wrapperLock(targetWrapper->mutex);

		targetWrapper->referenceCount--;
	}
}

void AirEngine::Core::IO::Manager::AssetManager::Collect()
{
	std::vector<AssetWrapper*> collectedWrappers = std::vector<AssetWrapper*>();
	{
		std::unique_lock<std::mutex> managerLock(_mutex);
		for (auto iter = _wrappers.begin(); iter != _wrappers.end(); )
		{
			std::unique_lock<std::mutex> wrapperLock(iter->second->mutex);
			if (iter->second->referenceCount == 0)
			{
				if (iter->second->isLoading)
				{
					iter++;
				}
				else
				{
					delete iter->second->asset;
					collectedWrappers.emplace_back(iter->second);
					iter = _wrappers.erase(iter);
				}
			}
			else
			{
				iter++;
			}
		}
	}
	for (const auto& wrapper : collectedWrappers)
	{
		delete wrapper;
	}
}