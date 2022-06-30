#pragma once
#include<thread>
namespace AirEngine
{
	namespace Utils
	{
		class ThreadBase
		{
		private:
			std::thread _thread;

		public:
			virtual void Init();
			void Start();
			void End();
			void WaitForStartFinish();
		protected:
			ThreadBase();
			virtual ~ThreadBase();
			virtual void OnStart();
			virtual void OnThreadStart();
			virtual void OnRun() = 0;
			virtual void OnEnd();
		private:
			bool _finishOnThreadStart;
			void Run();
			ThreadBase(const ThreadBase&) = delete;
			ThreadBase& operator=(const ThreadBase&) = delete;
			ThreadBase(ThreadBase&&) = delete;
			ThreadBase& operator=(ThreadBase&&) = delete;
		};
	}
}