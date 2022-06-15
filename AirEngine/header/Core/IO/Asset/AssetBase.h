#pragma once
#include <mutex>
#include <string>

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
			namespace Asset
			{
				class AssetBsse
				{
				private:
					bool _isHeldByManager;
					std::string _path;
				protected:
					virtual void OnLoad(Graphic::Command::CommandBuffer* transferCommndBuffer) = 0;
					AssetBsse(bool isHeldByManager);
					virtual ~AssetBsse();
				public:
					bool IsHeldByManager();
					std::string Path();
				};
			}
		}
	}
}