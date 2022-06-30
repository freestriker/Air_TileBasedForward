#pragma once
#include <mutex>
#include <string>
#include <rttr/registration>
#include <rttr/type>
#include "Core/Logic/Object/Object.h"

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
				class AssetWrapper;
			}
			namespace Asset
			{
				class AssetBase: public Core::Logic::Object::Object
				{
					friend class Manager::AssetManager;
				private:
					bool _isHeldByManager;
					std::string _path;
					Manager::AssetWrapper* _wrapper;
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