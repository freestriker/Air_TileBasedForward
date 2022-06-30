#include "Utils/ThreadBase.h"
#include <iostream>

void AirEngine::Utils::ThreadBase::OnStart()
{
}

void AirEngine::Utils::ThreadBase::OnThreadStart()
{
}

void AirEngine::Utils::ThreadBase::OnEnd()
{
}

AirEngine::Utils::ThreadBase::ThreadBase()
	: _thread()
	, _finishOnThreadStart(false)
{

}

AirEngine::Utils::ThreadBase::~ThreadBase()
{

}

void AirEngine::Utils::ThreadBase::Init()
{
}

void AirEngine::Utils::ThreadBase::Start()
{
	_finishOnThreadStart = false;
	OnStart();
	std::thread t(&ThreadBase::Run, this);
	_thread.swap(t);
}

void AirEngine::Utils::ThreadBase::End()
{
	OnEnd();
	if (_thread.joinable())
	{
		_thread.join();
	}
}

void AirEngine::Utils::ThreadBase::WaitForStartFinish()
{
	while (!_finishOnThreadStart)
	{
		std::this_thread::yield();
	}
}

void AirEngine::Utils::ThreadBase::Run()
{
	OnThreadStart();
	_finishOnThreadStart = true;
	OnRun();
}
