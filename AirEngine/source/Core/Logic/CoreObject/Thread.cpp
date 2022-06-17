#include "Core/Logic/CoreObject/Thread.h"
#include <QDebug>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Asset/Mesh.h"

AirEngine::Core::Logic::CoreObject::Thread::LogicThread AirEngine::Core::Logic::CoreObject::Thread::_logicThread = AirEngine::Core::Logic::CoreObject::Thread::LogicThread();

AirEngine::Core::Logic::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Logic::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Logic::CoreObject::Thread::Init()
{
	_logicThread.Init();
}

void AirEngine::Core::Logic::CoreObject::Thread::Start()
{
	_logicThread.Start();
}

void AirEngine::Core::Logic::CoreObject::Thread::End()
{
	_logicThread.End();
}

void AirEngine::Core::Logic::CoreObject::Thread::WaitForStartFinish()
{
	_logicThread.WaitForStartFinish();
}

AirEngine::Core::Logic::CoreObject::Thread::LogicThread::LogicThread()
	: _stopped(true)
{
}

AirEngine::Core::Logic::CoreObject::Thread::LogicThread::~LogicThread()
{
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::Init()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::Init()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnStart()
{
	_stopped = false;
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnStart()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnThreadStart()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnThreadStart()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnRun()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnRun()";
	auto meshTask = IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	while (!_stopped)
	{
		std::this_thread::yield();
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()";
}
