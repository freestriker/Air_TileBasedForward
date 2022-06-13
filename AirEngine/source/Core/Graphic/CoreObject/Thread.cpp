#include "Core/Graphic/CoreObject/Thread.h"
#include <QDebug>

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread AirEngine::Core::Graphic::CoreObject::Thread::_graphicThread = AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread();

AirEngine::Core::Graphic::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Graphic::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::Init()
{
	_graphicThread.Init();
}

void AirEngine::Core::Graphic::CoreObject::Thread::Start()
{
	_graphicThread.Start();
}

void AirEngine::Core::Graphic::CoreObject::Thread::End()
{
	_graphicThread.End();
}

void AirEngine::Core::Graphic::CoreObject::Thread::WaitForStartFinish()
{
	_graphicThread.WaitForStartFinish();
}

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::GraphicThread()
	: _stopped(true)
{
}

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::~GraphicThread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::Init()
{
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::Init()";;
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnStart()
{
	_stopped = false;
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnStart()";;
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnThreadStart()
{
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnThreadStart()";;
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnRun()
{
	while (!_stopped)
	{
		qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnRun()";;
		std::this_thread::yield();
	}
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnEnd()
{
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnEnd()";;
}
