#pragma once
#include <AL/al.h>
#include <AL/alext.h>
#include <al/alc.h>
#include <mutex>
#include <functional>

namespace AirEngine
{
	namespace Core
	{
		namespace Audio
		{
			namespace CoreObject
			{
				class Thread;
				class Instance final
				{
					friend class Thread;
				private:
					Instance();
					static void Init();

					static ALCdevice* _device;
					static ALCcontext* _context;
					static std::mutex _submitMutex;
				public:
					static void SubmitCommand(std::function<void()> submitFunction);
				};
			}
		}
	}
}
