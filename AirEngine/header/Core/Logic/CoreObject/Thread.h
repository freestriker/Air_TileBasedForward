#pragma once
#include <Utils/ThreadBase.h>
#include <mutex>
#include <vector>

namespace AirEngine
{
	namespace Core
	{
		namespace Logic
		{
			namespace CoreObject
			{
				class Thread final
				{
				private:
					class LogicThread final : public AirEngine::Utils::ThreadBase
					{
					public:
						bool _stopped;

						LogicThread();
						~LogicThread();
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
				private:
					static LogicThread _logicThread;
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
