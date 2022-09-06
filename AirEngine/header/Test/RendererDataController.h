#pragma once
#include "Behaviour/Behaviour.h"
#include <string>

namespace AirEngine
{
	namespace Test
	{
		class RendererDataController : public AirEngine::Behaviour::Behaviour
		{
		public:
			RendererDataController();
			~RendererDataController();
			RendererDataController(const RendererDataController&) = delete;
			RendererDataController& operator=(const RendererDataController&) = delete;
			RendererDataController(RendererDataController&&) = delete;
			RendererDataController& operator=(RendererDataController&&) = delete;
			void OnAwake()override;
			void OnStart()override;
			void OnUpdate()override;
			void OnDestroy()override;
			RTTR_ENABLE(AirEngine::Behaviour::Behaviour)
		};
	}
}
