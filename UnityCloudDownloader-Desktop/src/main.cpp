#include "systemtrayicon.h"

#include "unityclouddownloadercore.h"
#include "servicelocator.h"
#include "idatabaseprovider.h"
#include "logmanager.h"
#include "runguard.h"

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

    RunGuard runGuard(QStringLiteral("{6A26DD45-7CB9-4168-82CD-3D1C5305D582}"));
    if (!runGuard.tryLock())
        return 0;

    auto localPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir localDir(localPath);
    LogManagerGuard logManagerGuard(localDir.filePath("logs"));
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

    auto configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir configDir(configPath);
    if (!configDir.exists())
    {
        configDir.mkpath(configPath);
    }
    ucd::Core::init(configPath, &a);

    SystemTrayIcon trayIcon;
    trayIcon.show();

    QObject::connect(&runGuard, &RunGuard::triggered, &trayIcon, &SystemTrayIcon::configure);

    if (!ucd::ServiceLocator::databaseProvider()->hasProfiles())
    {
        trayIcon.configure();
    }

    runGuard.startWatch();
    return a.exec();
}
