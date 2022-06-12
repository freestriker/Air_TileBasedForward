#pragma once
#include <Utils/ThreadBase.h>
#include <mutex>
#include <vector>

namespace AirEngine
{
	namespace Core
	{
		namespace IO
		{
			namespace CoreObject
			{
				class Thread final
				{
				private:
					class IOThread final : public AirEngine::Utils::ThreadBase
					{
					public:
						bool _stopped;

						IOThread();
						~IOThread();
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
				private:
					static IOThread _ioThread;
					Thread();
					~Thread();
				public:
					static void Init();
					static void Start();
					static void End();
					static void WaitForStartFinish();
				};
			}
		}
	}
}
