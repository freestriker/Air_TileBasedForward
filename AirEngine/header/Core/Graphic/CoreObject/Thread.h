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
			}
			namespace CoreObject
			{
				class Thread final
				{
				private:
					class GraphicThread;
					class SubGraphicThread;
					static bool _stopped;
					static bool _canAddTask;

					static std::queue<std::function<void(AirEngine::Core::Graphic::Command::CommandPool*, AirEngine::Core::Graphic::Command::CommandPool*)>> _tasks;
					static std::mutex _taskMutex;
					static std::condition_variable _taskVariable;

					static GraphicThread _graphicThread;
					static std::array<SubGraphicThread, 4> _subGraphicThreads;

					class GraphicThread final : public AirEngine::Utils::ThreadBase
					{
					public:
						GraphicThread();
						~GraphicThread();
						GraphicThread(const GraphicThread&) = delete;
						GraphicThread& operator=(const GraphicThread&) = delete;
						GraphicThread(GraphicThread&&) = delete;
						GraphicThread& operator=(GraphicThread&&) = delete;
						void Init()override;
						void OnStart() override;
						void OnThreadStart() override;
						void OnRun() override;
						void OnEnd() override;
					};
					class SubGraphicThread final : public Utils::ThreadBase
					{
						friend class RenderThread;
						friend class Thread;
					private:
						std::mutex _mutex;
						Command::CommandPool* _graphicCommandPool;
						Command::CommandPool* _computeCommandPool;
					public:
						SubGraphicThread();
						~SubGraphicThread();
						SubGraphicThread(const SubGraphicThread&) = delete;
						SubGraphicThread& operator=(const SubGraphicThread&) = delete;
						SubGraphicThread(SubGraphicThread&&) = delete;
						SubGraphicThread& operator=(SubGraphicThread&&) = delete;

						void ResetCommandPool();

						void Init()override;
						void OnStart()override;
						void OnThreadStart()override;
						void OnRun()override;
						void OnEnd()override;
					};

				private:
					Thread();
					~Thread();
					Thread(const Thread&) = delete;
					Thread& operator=(const Thread&) = delete;
					Thread(Thread&&) = delete;
					Thread& operator=(Thread&&) = delete;
				public:
					static void Init();
					static void Start();
					static void WaitForStartFinish();
					static void End();

					template<typename F, typename... Args>
					static auto AddTask(F&& f, Args&&... args)->std::future<typename std::invoke_result<F, Command::CommandPool*, Command::CommandPool*, Args...>::type>;

				};
			}
		}
	}
}

template<typename F, typename... Args>
inline auto AirEngine::Core::Graphic::CoreObject::Thread::AddTask(F&& f, Args&&... args)->std::future<typename std::invoke_result<F, Command::CommandPool*, Command::CommandPool*, Args...>::type>
{
	using return_type = typename std::invoke_result<F, Graphic::Command::CommandPool*, Graphic::Command::CommandPool*, Args...>::type;

	auto task = std::make_shared<std::packaged_task<return_type(Graphic::Command::CommandPool*, Graphic::Command::CommandPool*)> >(
		std::bind(std::forward<F>(f), std::placeholders::_1, std::placeholders::_2, std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(_taskMutex);

		// don't allow enqueueing after stopping the pool
		Utils::Log::Exception("Can not add new graphic task when loadthread stopped.", !_canAddTask);

		_tasks.emplace([task](Graphic::Command::CommandPool* graphicCommandBuffer, Graphic::Command::CommandPool* computeCommandBuffer) { (*task)(graphicCommandBuffer, computeCommandBuffer); });
	}
	_taskVariable.notify_one();
	return res;
}
