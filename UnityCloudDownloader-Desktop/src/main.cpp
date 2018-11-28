#include "systemtrayicon.h"

#include "unityclouddownloadercore.h"
#include "servicelocator.h"
#include "idatabaseprovider.h"
#include "logmanager.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Valtech");
    QCoreApplication::setOrganizationDomain("valtech.com");
    QCoreApplication::setApplicationName("UnityCloudDownloader");

    auto configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir configDir(configPath);
    if (!configDir.exists())
    {
        configDir.mkpath(configPath);
    }

    LogManagerGuard logManagerGuard(configDir.filePath("logs"));
    Q_UNUSED(logManagerGuard);
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

    ucd::Core::init(configPath, &a);

    SystemTrayIcon trayIcon;
    trayIcon.show();

    if (!ucd::ServiceLocator::databaseProvider()->hasProfiles())
    {
        trayIcon.configure();
    }

    return a.exec();
}
