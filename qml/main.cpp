#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QString>

#include "robot_backend.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    RobotBackend robot;

    engine.rootContext()->setContextProperty("robot", &robot);

    engine.load(QUrl::fromLocalFile(QStringLiteral(SOURCE_DIR "/qml/Main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
