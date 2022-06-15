#include "Core/IO/CoreObject/Thread.h"
#include <QDebug>
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/Command/CommandBuffer.h"

AirEngine::Core::IO::CoreObject::Thread::IOThread AirEngine::Core::IO::CoreObject::Thread::_ioThread = AirEngine::Core::IO::CoreObject::Thread::IOThread();
std::array<AirEngine::Core::IO::CoreObject::Thread::SubIOThread, 4> AirEngine::Core::IO::CoreObject::Thread::_subIOThreads = std::array<AirEngine::Core::IO::CoreObject::Thread::SubIOThread, 4>();

AirEngine::Core::IO::CoreObject::Thread::Thread()
{
}

AirEngine::Core::IO::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::Init()
{
	_ioThread.Init();
	for (auto& subThread : _subIOThreads)
	{
		subThread.Init();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::Start()
{
	_ioThread.Start();
	_ioThread.WaitForStartFinish();

	for (auto& subThread : _subIOThreads)
	{
		subThread.Start();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::WaitForStartFinish()
{
	for (auto& subThread : _subIOThreads)
	{
		subThread.WaitForStartFinish();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::End()
{
	_ioThread._canAddTask = false;
	for (auto& subThread : _subIOThreads)
	{
		subThread.End();
	}
	_ioThread.End();
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::IOThread()
	: _stopped(true)
	, _canAddTask(false)
	, tasks()
	, taskMutex()
	, taskVariable()
{
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::~IOThread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::Init()
{
	_stopped = true;
	_canAddTask = false;
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::Init()";
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnStart()
{
	_stopped = false;
	_canAddTask = true;
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnStart()";
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnThreadStart()
{
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnThreadStart()";
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnRun()
{
	while (!_stopped)
	{
		qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnRun()";
		std::this_thread::yield();
	}
	//clear assets
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnEnd()
{
	_stopped = true;
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnEnd()";
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
			std::unique_lock<std::mutex> lock(_ioThread.taskMutex);
			_ioThread.taskVariable.wait(lock, [this] { return !_ioThread._canAddTask || !_ioThread.tasks.empty(); });
			if (!_ioThread._canAddTask && _ioThread.tasks.empty())
			{
				return;
			}
			task = std::move(_ioThread.tasks.front());
			_ioThread.tasks.pop();
		}

		task(_transferCommandBuffer);
	}
}

void AirEngine::Core::IO::CoreObject::Thread::SubIOThread::OnEnd()
{
}
