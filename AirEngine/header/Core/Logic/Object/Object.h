#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>

#define CONSTRUCTOR(T) \
	T();\
	virtual ~T();\
	T(const T&) = delete;\
	T& operator=(const T&) = delete;\
	T(T&&) = delete;\
	T& operator=(T&&) = delete;

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