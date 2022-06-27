#pragma once
#include "Core/Logic/Object/Component.h"

namespace AirEngine
{
	namespace Behaviour
	{
		class Behaviour: public Core::Logic::Object::Component
		{
		public:
			Behaviour();
			~Behaviour();
			virtual void OnAwake()override;
			virtual void OnStart()override;
			virtual void OnUpdate()override;
			virtual void OnDestroy()override;
			Behaviour(const Behaviour&) = delete;
			Behaviour& operator=(const Behaviour&) = delete;
			Behaviour(Behaviour&&) = delete;
			Behaviour& operator=(Behaviour&&) = delete;
		};
	}
}