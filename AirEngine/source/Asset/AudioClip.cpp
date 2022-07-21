#include "Asset/AudioClip.h"
#include <filesystem>
#include <fstream>
#include "Core/Audio/CoreObject/Instance.h"
#include "Utils/Log.h"

AirEngine::Asset::AudioClip::AudioClip()
	: AssetBase(true)
	, _buffer()
    , _bytes()
{
}

AirEngine::Asset::AudioClip::~AudioClip()
{
}

void AirEngine::Asset::AudioClip::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
    // open the file:
    std::ifstream file(Path(), std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    _bytes.resize(fileSize);
    file.read(_bytes.data(), fileSize);

    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this]()->void
        {
            alGenBuffers(1, &this->_buffer);
            Utils::Log::Exception("Failed to create audio clip.", alGetError());
            alBufferData(this->_buffer, AL_FORMAT_STEREO16, this->_bytes.data(), this->_bytes.size(), 44100);
            Utils::Log::Exception("Failed to create audio clip.", alGetError());
        }
    );
}

