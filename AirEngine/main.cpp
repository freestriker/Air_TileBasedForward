#include <QApplication>
#include "Core/Graphic/CoreObject/Window.h"
#include <qmainwindow.h>
#include <qpushbutton.h>
#include "Utils/Log.h"

Q_LOGGING_CATEGORY(lcVk, "qt.vulkan")
//
//class MainWindow : public QMainWindow
//{
//    Q_OBJECT
//private:
//    QPushButton button;
//private:
//public:
//    MainWindow(QWidget* parent = nullptr)
//        :button(this)
//    {
//        QObject::connect(&button, &QPushButton::clicked, this, []()->void {
//            AirEngine::Utils::Log::Message("Clicked.");
//        });
//    }
//    ~MainWindow() = default;
//
//};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AirEngine::Core::Graphic::CoreObject::Window::Init();
    AirEngine::Core::Graphic::CoreObject::Window::Start();
    //auto&& mainWindow = new MainWindow();
    //mainWindow->show();
    return app.exec();
}

//#include "main.moc"