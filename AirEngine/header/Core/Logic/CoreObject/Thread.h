#pragma once
#include <Utils/ThreadBase.h>
#include <mutex>
#include <vector>
#include <Core/Logic/Object/Component.h>

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
					private:
						void IterateByDynamicBfs(Object::Component::ComponentType targetComponentType);
						void IterateByStaticBfs(Object::Component::ComponentType targetComponentType);
						void IterateByStaticBfs(Object::Component::ComponentType targetComponentType, std::vector<Object::Component*>& targetComponents);
						void IterateByStaticBfs(std::vector<Object::Component::ComponentType> targetComponentTypes);
						void IterateByStaticBfs(std::vector<Object::Component::ComponentType> targetComponentTypes, std::vector < std::vector<Object::Component*>>& targetComponents);
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
