#pragma once
#include <rttr/type.h>
#include <mutex>
#include <map>
#include <string>
#include "Core/IO/CoreObject/Thread.h"
#include "Core/IO/Asset/AssetBase.h"

namespace AirEngine
{
	namespace Core
	{
		namespace IO
		{
			namespace Manager
			{
				class AssetManager final
				{
				private:
					struct AssetWarp
					{
						std::string path;
						Core::IO::Asset::AssetBase* asset;
						uint32_t referenceCount;
						std::mutex mutex;
					};
					struct AssetPool
					{
						rttr::type type;
						std::mutex mutex;
						std::map<std::string, AssetWarp*> assetWarps;
					};
					std::map<std::string, AssetPool*> _pools;
					std::mutex _mutex;
				public:
					AssetManager();
					~AssetManager();
					AssetManager(const AssetManager&) = delete;
					AssetManager& operator=(const AssetManager&) = delete;
					AssetManager(AssetManager&&) = delete;
					AssetManager& operator=(AssetManager&&) = delete;

					template<typename TAsset>
					TAsset* LoadAsset(std::string path);
					template<typename TAsset>
					void UnloadAsset(TAsset*& asset);
				};

				template<typename TAsset>
				inline TAsset* AssetManager::LoadAsset(std::string path)
				{
					rttr::type type = rttr::type::get<TAsset>();
					std::string typeName = type.get_name().to_string();

					{
						//Get target pool
						std::unique_lock<std::mutex> managerLock(_mutex);
						auto poolIter = _pools.find(typeName);
						AssetPool* targetPool = nullptr;
						if (poolIter == std::end(_pools))
						{
							targetPool = new AssetPool();
							targetPool->type = type;
							_pools.emplace({ typeName, targetPool });
						}
						else
						{
							targetPool = poolIter->second;
						}

						//Check pool
						{
							std::unique_lock<std::mutex> poolLock(targetPool->mutex);

							auto assetIter = targetPool->assetWarps.find(path);
							AssetWarp* targetWarp = nullptr;
							if (assetIter == std::end(targetPool->assetWarps))
							{
								targetWarp = new AssetWarp();
								targetWarp->path = path;
								targetWarp->referenceCount = 0;
								targetWarp->asset = nullptr;

								targetPool->assetWarps.emplace({ path , targetWarp });
							}
							else
							{
								targetWarp = assetIter->second;
							}

							//load
							{
								std::unique_lock<std::mutex> warpLock(targetWarp->mutex);
								TAsset*& targetAsset = targetWarp->asset;
								targetWarp->referenceCount++;

								if (targetAsset == nullptr)
								{
									targetAsset = new TAsset();
									std::future<Asset::AssetBase*> task = CoreObject::Thread::AddTask([targetWarp](Core::Graphic::Command::CommandBuffer* transferCommandBuffer)->void {
										std::unique_lock<std::mutex> warpLock(targetWarp->mutex);
										Asset::AssetBase* assetBase = dynamic_cast<Asset::AssetBase*>(targetWarp);
										assetBase->OnLoad(transferCommandBuffer);
										return assetBase;
									});
									return dynamic_cast<TAsset*>(task.get());
								}
								else
								{
									return dynamic_cast<TAsset*>(targetAsset);
								}
							}
						}
					}
				}
				template<typename TAsset>
				inline void AssetManager::UnloadAsset(TAsset*& asset)
				{
				}
			}
		}
	}
}