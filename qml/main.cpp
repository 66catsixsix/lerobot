#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QString>
#include "robot_backend.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<RobotBackend>("LeRobot",1,0,"RobotBackend");
    QQmlApplicationEngine engine;
    engine.load(QUrl::fromLocalFile(QStringLiteral(SOURCE_DIR "/qml/Main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
