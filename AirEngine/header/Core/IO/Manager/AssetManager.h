#pragma once
#include <rttr/type.h>
#include <mutex>
#include <map>
#include <string>
#include "Core/IO/CoreObject/Thread.h"
#include "Core/IO/Asset/AssetBase.h"
#include <functional>
#include <string>
#include <vector>
#include <fstream>

namespace AirEngine
{
	namespace Core
	{
		namespace IO
		{
			namespace Manager
			{
				struct AssetWrapper
				{
					std::string path;
					Core::IO::Asset::AssetBase* asset;
					uint32_t referenceCount;
					std::mutex mutex;
					bool isLoading;
				};

				class AssetManager final
				{
				private:
					std::unordered_map<std::string, AssetWrapper*> _wrappers;
					std::mutex _mutex;
				public:
					AssetManager();
					~AssetManager();
					AssetManager(const AssetManager&) = delete;
					AssetManager& operator=(const AssetManager&) = delete;
					AssetManager(AssetManager&&) = delete;
					AssetManager& operator=(AssetManager&&) = delete;

					template<typename TAsset>
					std::future< TAsset*> LoadAsync(std::string path);
					template<typename TAsset>
					TAsset* Load(std::string path);
					void Unload(Asset::AssetBase*& asset);
					void Unload(std::string path);
					void Collect();
				};

				template<typename TAsset>
				std::future< TAsset*> AssetManager::LoadAsync(std::string path)
				{
					{
						std::ifstream f(path);
						Utils::Log::Exception("Can not find file: " + path + " .", !f.good());
					}
					{
						std::unique_lock<std::mutex> managerLock(_mutex);

						//Get target pool
						AssetWrapper* targetWrapper = nullptr;
						auto wrapperIter = _wrappers.find(path);
						if (wrapperIter == std::end(_wrappers))
						{
							targetWrapper = new AssetWrapper();
							targetWrapper->path = path;
							targetWrapper->referenceCount = 0;
							targetWrapper->asset = nullptr;
							targetWrapper->isLoading = true;

							_wrappers.emplace(path, targetWrapper);
						}
						else
						{
							targetWrapper = wrapperIter->second;
						}


						{
							std::unique_lock<std::mutex> wrapperLock(targetWrapper->mutex);

							//Check
							targetWrapper->referenceCount++;
							if (targetWrapper->asset)
							{
								return std::async([targetWrapper]()
								{
									while (targetWrapper->isLoading)
									{
										std::this_thread::yield();
									}
									return dynamic_cast<TAsset*>(targetWrapper->asset);
								});
							}
							else
							{
								Asset::AssetBase* asset = dynamic_cast<Asset::AssetBase*>(new TAsset());
								targetWrapper->asset = asset;
								asset->_wrapper = targetWrapper;
								asset->_path = path;

								return CoreObject::Thread::AddTask
								(
									[targetWrapper](Core::Graphic::Command::CommandBuffer* transferCommandBuffer)->TAsset*
									{
										targetWrapper->asset->OnLoad(transferCommandBuffer);
										targetWrapper->isLoading = false;
										return dynamic_cast<TAsset*>(targetWrapper->asset);
									}
								);
							}
						}
					}

				}
				template<typename TAsset>
				TAsset* AssetManager::Load(std::string path)
				{
					return LoadAsync<TAsset>(path).get();
				}
			}
		}
	}
}