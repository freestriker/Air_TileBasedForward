#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Core/Graphic/CoreObject/Window.h"

Q_LOGGING_CATEGORY(lcVk, "qt.vulkan")

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    AirEngine::Core::Graphic::CoreObject::Window::Init();
    AirEngine::Core::Graphic::CoreObject::Window::Start();

    return app.exec();
}
