#include "Core/IO/CoreObject/Thread.h"
#include <QDebug>

AirEngine::Core::IO::CoreObject::Thread::IOThread AirEngine::Core::IO::CoreObject::Thread::_ioThread = AirEngine::Core::IO::CoreObject::Thread::IOThread();

AirEngine::Core::IO::CoreObject::Thread::Thread()
{
}

AirEngine::Core::IO::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::Init()
{
	_ioThread.Init();
}

void AirEngine::Core::IO::CoreObject::Thread::Start()
{
	_ioThread.Start();
}

void AirEngine::Core::IO::CoreObject::Thread::End()
{
	_ioThread.End();
}

void AirEngine::Core::IO::CoreObject::Thread::WaitForStartFinish()
{
	_ioThread.WaitForStartFinish();
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::IOThread()
	: _stopped(true)
{
}

AirEngine::Core::IO::CoreObject::Thread::IOThread::~IOThread()
{
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::Init()
{
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::Init()";;
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnStart()
{
	_stopped = false;
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnStart()";;
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnThreadStart()
{
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnThreadStart()";;
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnRun()
{
	while (!_stopped)
	{
		qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnRun()";;
		std::this_thread::yield();
	}
}

void AirEngine::Core::IO::CoreObject::Thread::IOThread::OnEnd()
{
	qDebug() << "AirEngine::Core::IO::CoreObject::Thread::IOThread::OnEnd()";;
}