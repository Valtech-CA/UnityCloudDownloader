#include "systemtrayicon.h"

#include "unityclouddownloadercore.h"
#include "database.h"

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

    ucd::Core::init();

    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

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
    auto configFilePath = configDir.filePath("data.sqlite");

    ucd::Database database(configFilePath);
    database.init();

    SystemTrayIcon trayIcon;
    trayIcon.show();

    return a.exec();
}
