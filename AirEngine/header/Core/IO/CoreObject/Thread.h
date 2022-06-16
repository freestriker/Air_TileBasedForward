#pragma once
#include "Utils/ThreadBase.h"
#include <mutex>
#include <vector>
#include <queue>
#include <future>
#include "Utils/Log.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandPool;
				class CommandBuffer;
			}
		}
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
						IOThread();
						~IOThread();
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
					class SubIOThread final: public Utils::ThreadBase
					{
					private:
						Graphic::Command::CommandPool* _transferCommandPool;
						Graphic::Command::CommandBuffer* _transferCommandBuffer;
					public:
						SubIOThread();
						~SubIOThread();

						void Init()override;
						void OnStart()override;
						void OnThreadStart() override;
						void OnRun()override;
						void OnEnd()override;
					};

				private:
					static bool _stopped;
					static bool _canAddTask;

					static std::queue<std::function<void(Graphic::Command::CommandBuffer*)>> tasks;
					static std::mutex taskMutex;
					static std::condition_variable taskVariable;

					static IOThread _ioThread;
					static std::array<SubIOThread, 4> _subIOThreads;
					Thread();
					~Thread();
				public:
					static void Init();
					static void Start();
					static void End();
					static void WaitForStartFinish();

					template<typename F, typename... Args>
					static auto AddTask(F&& f, Args&&... args)->std::future<typename std::invoke_result<F, Graphic::Command::CommandBuffer*, Args...>::type>;
				};

				template<typename F, typename ...Args>
				inline auto Thread::AddTask(F&& f, Args && ...args) -> std::future<typename std::invoke_result<F, Graphic::Command::CommandBuffer*, Args ...>::type>
				{
					using return_type = typename std::invoke_result<F, Graphic::Command::CommandBuffer*, Args...>::type;

					auto task = std::make_shared<std::packaged_task<return_type(Graphic::Command::CommandBuffer*)> >(
						std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Args>(args)...)
						);

					std::future<return_type> res = task->get_future();
					{
						std::unique_lock<std::mutex> lock(taskMutex);

						// don't allow enqueueing after stopping the pool
						Utils::Log::Exception("Can not add new io task when loadthread stopped.", _canAddTask);

						tasks.emplace([task](Graphic::Command::CommandBuffer* const transferCommandBuffer) { (*task)(transferCommandBuffer); });
					}
					taskVariable.notify_one();
					return res;
				}
			}
		}
	}
}
