#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>

namespace AirEngine
{
	namespace Core
	{
		namespace Logic
		{
			namespace Object
			{
				class Object
				{
				public:
					Object();
					virtual ~Object();
					rttr::type Type();
					virtual std::string ToString();
				protected:
					virtual void OnDestroy();
					virtual void OnAwake();
					RTTR_ENABLE()
				};
			}
		}
	}
}