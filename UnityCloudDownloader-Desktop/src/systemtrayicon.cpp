#include "systemtrayicon.h"

#include <QApplication>
#include <QMenu>
#include <QAction>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_menu(nullptr)
    , m_quitAction(nullptr)
{
    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);

    m_menu = new QMenu();
    m_menu->addAction(m_quitAction);

    setContextMenu(m_menu);
    setToolTip(tr("Unity Cloud Downloader"));
    setIcon(QIcon(":/icons/UnityCloudLogo2.png"));
}

SystemTrayIcon::~SystemTrayIcon()
{}
