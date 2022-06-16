#pragma once
#include <mutex>
#include <string>
#include <rttr/registration>
#include <rttr/type>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandBuffer;
			}
		}
		namespace IO
		{
			namespace Manager
			{
				class AssetManager;
			}
			namespace Asset
			{
				class AssetBase
				{
					friend class Manager::AssetManager;
				private:
					bool _isHeldByManager;
					std::string _path;
				protected:
					virtual void OnLoad(Graphic::Command::CommandBuffer* transferCommndBuffer) = 0;
					AssetBase(bool isHeldByManager);
					virtual ~AssetBase();
				public:
					bool IsHeldByManager();
					std::string Path();

					RTTR_ENABLE()
				};
			}
		}
	}
}