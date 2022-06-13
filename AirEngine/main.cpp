#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Core/Logic/CoreObject/Thread.h"
#include "Core/IO/CoreObject/Thread.h"
#include "Core/Graphic/CoreObject/Thread.h"
#include "Core/Graphic/CoreObject/Window.h"

Q_LOGGING_CATEGORY(lcVk, "qt.vulkan")

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    AirEngine::Core::Graphic::CoreObject::Window::Init();
    AirEngine::Core::Graphic::CoreObject::Thread::Init();
    AirEngine::Core::Logic::CoreObject::Thread::Init();
    AirEngine::Core::IO::CoreObject::Thread::Init();

    AirEngine::Core::Graphic::CoreObject::Window::Start();
    AirEngine::Core::Graphic::CoreObject::Thread::Start();
    AirEngine::Core::Logic::CoreObject::Thread::Start();
    AirEngine::Core::IO::CoreObject::Thread::Start();
    return app.exec();
}
