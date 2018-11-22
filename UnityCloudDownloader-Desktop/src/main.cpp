#include "systemtrayicon.h"

#include "unityclouddownloadercore.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Valtech");
    QCoreApplication::setOrganizationDomain("valtech.com");
    QCoreApplication::setApplicationName("UnityCloudDownloader");

    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(true);

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(
                nullptr,
                QObject::tr("Systray"),
                QObject::tr("Couldn't detect any system tray on this system."));
        return 1;
    }

    auto configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir configDir(configPath);
    if (!configDir.exists())
    {
        configDir.mkpath(configPath);
    }
    ucd::Core::init(configPath, &a);

    SystemTrayIcon trayIcon;
    trayIcon.show();

    QMetaObject::invokeMethod(&trayIcon, "onConfigure");

    return a.exec();
}
