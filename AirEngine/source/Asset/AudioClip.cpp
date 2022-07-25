#include "Asset/AudioClip.h"
#include <filesystem>
#include <fstream>
#include "Core/Audio/CoreObject/Instance.h"
#include "Utils/Log.h"

AirEngine::Asset::AudioClip::AudioClip()
	: AssetBase(true)
	, _buffer()
    , _format()
    , _sfInfo()
    , _frameCount()
    , _byteCount()
{
}

AirEngine::Asset::AudioClip::~AudioClip()
{
}

double AirEngine::Asset::AudioClip::Duration()
{
    return static_cast<double>(_frameCount) / _sfInfo.samplerate;
}

ALuint AirEngine::Asset::AudioClip::Buffer()
{
    return _buffer;
}

ALenum AirEngine::Asset::AudioClip::Formart()
{
    return _format;
}

SF_INFO AirEngine::Asset::AudioClip::Info()
{
    return _sfInfo;
}

sf_count_t AirEngine::Asset::AudioClip::FrameCount()
{
    return _frameCount;
}

void AirEngine::Asset::AudioClip::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
    /* Open the audio file and check that it's usable. */
    SNDFILE* sndfile = sf_open(Path().c_str(), SFM_READ, &_sfInfo);
    Utils::Log::Exception("Failed to open audio file: " + Path() + ", error code: " + sf_strerror(sndfile) + ".", !sndfile);
    Utils::Log::Exception("Bad sample count audio file: " + Path() + ".", _sfInfo.frames < 1 || _sfInfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / _sfInfo.channels);

    /* Get the sound format, and figure out the OpenAL format */
    _format = AL_NONE;
    switch (_sfInfo.channels)
    {
        case 1:
        {
            _format = AL_FORMAT_MONO16;
            break;
        }
        case 2:
        {
            _format = AL_FORMAT_STEREO16;
            break;
        }
        case 3:
        {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            {
                _format = AL_FORMAT_BFORMAT2D_16;
            }
            break;
        }
        case 4:
        {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            {
                _format = AL_FORMAT_BFORMAT3D_16;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    Utils::Log::Exception("Unsupported channel count in file: " + Path() + ".", !_format);

    /* Decode the whole audio file to a buffer. */
    short* memoryBuffer = reinterpret_cast<short*>(malloc((size_t)(_sfInfo.frames * _sfInfo.channels) * sizeof(short)));

    _frameCount = sf_readf_short(sndfile, memoryBuffer, _sfInfo.frames);
    Utils::Log::Exception("Failed to read samples in file: " + Path() + ".", _frameCount < 1);

    _byteCount = (ALsizei)(_frameCount * _sfInfo.channels) * (ALsizei)sizeof(short);

    /* Buffer the audio data into a new buffer object, then free the data and
     * close the file.
     */
    Core::Audio::CoreObject::Instance::SubmitCommand(
        [this, memoryBuffer]()->void
        {
            Utils::Log::Exception("Previous error in create audio clip.", alGetError());
            alGenBuffers(1, &this->_buffer);
            Utils::Log::Exception("Failed to create audio clip.", alGetError());
            alBufferData(this->_buffer, this->_format, memoryBuffer, this->_byteCount, this->_sfInfo.samplerate);
            Utils::Log::Exception("Failed to create audio clip.", alGetError());
        }
    );

    free(memoryBuffer);
    sf_close(sndfile);
}

