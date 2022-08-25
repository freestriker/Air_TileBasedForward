#include "Core/IO/CoreObject/Thread.h"
#include <QDebug>
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"

AirEngine::Core::IO::CoreObject::Thread::IOThread AirEngine::Core::IO::CoreObject::Thread::_ioThread = AirEngine::Core::IO::CoreObject::Thread::IOThread();
std::array<AirEngine::Core::IO::CoreObject::Thread::SubIOThread, 4> AirEngine::Core::IO::CoreObject::Thread::_subIOThreads = std::array<AirEngine::Core::IO::CoreObject::Thread::SubIOThread, 4>();

bool AirEngine::Core::IO::CoreObject::Thread::_stopped = true;
bool AirEngine::Core::IO::CoreObject::Thread::_canAddTask = false;

std::mutex AirEngine::Core::IO::CoreObject::Thread::taskMutex = std::mutex();
std::condition_variable AirEngine::Core::IO::CoreObject::Thread::taskVariable = std::condition_variable();
std::queue<std::function<void(AirEngine::Core::Graphic::Command::CommandBuffer*)>> AirEngine::Core::IO::CoreObject::Thread::tasks = std::queue<std::function<void(AirEngine::Core::Graphic::Command::CommandBuffer*)>>();

AirEngine::Core::IO::CoreObject::Thread::Thread()
{
}

AirEngine::Core::IO::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::Init()
{
	_stopped = true;
	_canAddTask = false;

	_ioThread.Init();

	for (auto& subThread : _subIOThreads)
	{
		subThread.Init();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::Start()
{
	_stopped = false;
	_ioThread.Start();
	_ioThread.WaitForStartFinish();

	_canAddTask = true;
	for (auto& subThread : _subIOThreads)
	{
		subThread.Start();
		subThread.WaitForStartFinish();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::WaitForStartFinish()
{

}

void AirEngine::Core::IO::CoreObject::Thread::End()
{
	_canAddTask = false;
	for (auto& subThread : _subIOThreads)
	{
		subThread.End();
	}

	_stopped = true;
	_ioThread.End();
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::IOThread()
{
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::~IOThread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::Init()
{

}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnStart()
{
	Instance::Init();

}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnThreadStart()
{

}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnRun()
{
	while (!_stopped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		Instance::AssetManager().Collect();
	}
	Instance::AssetManager().Collect();
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnEnd()
{

}

AirEngine::Core::IO::CoreObject::Thread::SubIOThread::SubIOThread()
	: _transferCommandPool(nullptr)
	, _transferCommandBuffer(nullptr)
{
}

AirEngine::Core::IO::CoreObject::Thread::SubIOThread::~SubIOThread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::Init()
{
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::OnStart()
{
	_transferCommandPool = new Core::Graphic::Command::CommandPool("TransferQueue", VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	_transferCommandBuffer = _transferCommandPool->CreateCommandBuffer("TransferCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::OnThreadStart()
{
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::OnRun()
{
	while (true)
	{
		std::function<void(Graphic::Command::CommandBuffer* const)> task;

		{
			std::unique_lock<std::mutex> lock(taskMutex);
			taskVariable.wait(lock, [this] { return !_canAddTask || !tasks.empty(); });
			if (!_canAddTask && tasks.empty())
			{
				return;
			}
			task = std::move(tasks.front());
			tasks.pop();
		}

		task(_transferCommandBuffer);
	}
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::OnEnd()
{
}
