#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Core/Logic/CoreObject/Thread.h"
#include "Core/IO/CoreObject/Thread.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    AirEngine::Core::Logic::CoreObject::Thread::Init();
    AirEngine::Core::IO::CoreObject::Thread::Init();

    AirEngine::Core::Logic::CoreObject::Thread::Start();
    AirEngine::Core::IO::CoreObject::Thread::Start();
    return app.exec();
}
