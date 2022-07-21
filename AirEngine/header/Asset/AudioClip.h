#pragma once
#include "Core/IO/Asset/AssetBase.h"
#include <rttr/type>
#include <AL/al.h>
#include <vector>

namespace AirEngine
{
	namespace Asset
	{
		class AudioClip final : public Core::IO::Asset::AssetBase
		{
		public:
		private:
			ALuint _buffer;
			std::vector<char> _bytes;
			void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
		public:
			AudioClip();
			~AudioClip();

			RTTR_ENABLE(AirEngine::Core::IO::Asset::AssetBase)
		};
	}
}