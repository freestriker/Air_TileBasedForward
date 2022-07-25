#pragma once
#include <Utils/ThreadBase.h>
#include <mutex>
#include <vector>

namespace AirEngine
{
	namespace Core
	{
		namespace Audio
		{
			namespace CoreObject
			{
				class Thread final
				{
				private:
					class AudioThread final : public AirEngine::Utils::ThreadBase
					{
					private:
					public:
						bool _stopped;

						AudioThread();
						~AudioThread();
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
				private:
					static AudioThread _audioThread;
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
