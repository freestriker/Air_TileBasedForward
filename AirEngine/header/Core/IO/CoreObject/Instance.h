#pragma once
namespace AirEngine
{
	namespace Core
	{
		namespace IO
		{
			namespace Manager
			{
				class AssetManager;
			}
			namespace CoreObject
			{
				class Thread;
				class Instance final
				{
					friend class Thread;
				private:
					static Manager::AssetManager* _assetManager;
					Instance();
					static void Init();
				public:
					static Manager::AssetManager& AssetManager();
				};
			}
		}
	}
}
