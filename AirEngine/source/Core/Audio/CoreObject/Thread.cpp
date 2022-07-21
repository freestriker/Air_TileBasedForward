#include "Core/Audio/CoreObject/Thread.h"
#include <QDebug>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/Audio/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Utils/Condition.h"
#include "Core/Graphic/CoreObject/Instance.h"

AirEngine::Core::Audio::CoreObject::Thread::AudioThread AirEngine::Core::Audio::CoreObject::Thread::_audioThread = AirEngine::Core::Audio::CoreObject::Thread::AudioThread();

AirEngine::Core::Audio::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Audio::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Audio::CoreObject::Thread::Init()
{
	_audioThread.Init();
}

void AirEngine::Core::Audio::CoreObject::Thread::Start()
{
	_audioThread.Start();
}

void AirEngine::Core::Audio::CoreObject::Thread::End()
{
	_audioThread.End();
}

void AirEngine::Core::Audio::CoreObject::Thread::WaitForStartFinish()
{
	_audioThread.WaitForStartFinish();
}

AirEngine::Core::Audio::CoreObject::Thread::AudioThread::AudioThread()
	: _stopped(true)
{
}

AirEngine::Core::Audio::CoreObject::Thread::AudioThread::~AudioThread()
{
}

void AirEngine::Core::Audio::CoreObject::Thread::AudioThread::Init()
{
	Utils::Log::Message("AirEngine::Core::Audio::CoreObject::Thread::AudioThread::Init()");
}

void AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnStart()
{
	_stopped = false;
	Instance::Init();
	Utils::Log::Message("AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnStart()");
}

void AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnThreadStart()
{
	Utils::Log::Message("AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnThreadStart()");
}

void AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnRun()
{
	Utils::Log::Message("AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnRun()");
	//while (!_stopped)
	//{

	//}
}

void AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnEnd()
{
	Utils::Log::Message("AirEngine::Core::Audio::CoreObject::Thread::AudioThread::OnEnd()");
}
