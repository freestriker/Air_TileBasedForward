#include "Core/Audio/CoreObject/Instance.h"
#include "Utils/Log.h"

ALCdevice* AirEngine::Core::Audio::CoreObject::Instance::_device = nullptr;
ALCcontext* AirEngine::Core::Audio::CoreObject::Instance::_context = nullptr;

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
