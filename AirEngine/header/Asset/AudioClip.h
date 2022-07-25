#pragma once
#include "Core/IO/Asset/AssetBase.h"
#include <rttr/type>
#include <AL/al.h>
#include <vector>
#include <sndfile.hh>

namespace AirEngine
{
	namespace Asset
	{
		class AudioClip final : public Core::IO::Asset::AssetBase
		{
		public:
		private:
			ALuint _buffer;
			ALenum _format;
			SF_INFO _sfInfo;
			sf_count_t _frameCount;
			ALsizei _byteCount;
			void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
		public:
			AudioClip();
			~AudioClip();
			double Duration();
			ALuint Buffer();
			ALenum Formart();
			SF_INFO Info();
			sf_count_t FrameCount();

			RTTR_ENABLE(AirEngine::Core::IO::Asset::AssetBase)
		};
	}
}