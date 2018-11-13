#include "systemtrayicon.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

int main(int argc, char *argv[])
{
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

    QCoreApplication::setOrganizationName("Valtech");
    QCoreApplication::setOrganizationDomain("valtech.com");
    QCoreApplication::setApplicationName("UnityCloudDownloader");

    SystemTrayIcon trayIcon;
    trayIcon.show();

    return a.exec();
}
