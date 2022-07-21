#include "Core/Audio/CoreObject/Instance.h"
#include "Utils/Log.h"

ALCdevice* AirEngine::Core::Audio::CoreObject::Instance::_device = nullptr;
ALCcontext* AirEngine::Core::Audio::CoreObject::Instance::_context = nullptr;
std::mutex AirEngine::Core::Audio::CoreObject::Instance::_submitMutex = std::mutex();

AirEngine::Core::Audio::CoreObject::Instance::Instance()
{

}

void AirEngine::Core::Audio::CoreObject::Instance::Init()
{
    _device = alcOpenDevice(NULL);
    Utils::Log::Exception("Failed to open audio device.", !_device);
    _context = alcCreateContext(_device, NULL);
    Utils::Log::Exception("Failed to create audio context.", !_context);
    alcMakeContextCurrent(_context);
    Utils::Log::Exception("Failed to create audio context.", alGetError());
}

void AirEngine::Core::Audio::CoreObject::Instance::SubmitCommand(std::function<void()> submitFunction)
{
    std::lock_guard<std::mutex> locker = std::lock_guard<std::mutex>(_submitMutex);
    submitFunction();
}