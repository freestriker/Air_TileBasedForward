#pragma once
#include <Utils/ThreadBase.h>
#include <mutex>
#include <vector>

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Thread final
				{
				private:
					class GraphicThread final : public AirEngine::Utils::ThreadBase
					{
					public:
						bool _stopped;

						GraphicThread();
						~GraphicThread();
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
				private:
					static GraphicThread _graphicThread;
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
