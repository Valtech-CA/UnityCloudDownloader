#include "systemtrayicon.h"

#include "qmlcontext.h"

#include "database.h"
#include "unityapiclient.h"
#include "profilesmodel.h"

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>

SystemTrayIcon::SystemTrayIcon(ucd::Database *data, QObject *parent)
    : QSystemTrayIcon(parent)
    , m_db(data)
    , m_menu(nullptr)
    , m_quitAction(nullptr)
    , m_qmlEngine(nullptr)
    , m_window(nullptr)
{
    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);

    auto configureAction = new QAction(tr("&Configure"), this);
    connect(configureAction, &QAction::triggered, this, &SystemTrayIcon::onConfigure);

    m_menu = new QMenu();
    m_menu->addAction(configureAction);
    m_menu->addSeparator();
    m_menu->addAction(m_quitAction);

    setContextMenu(m_menu);
    setToolTip(tr("Unity Cloud Downloader"));
    setIcon(QIcon(":/icons/UnityCloudLogo2.png"));
}

SystemTrayIcon::~SystemTrayIcon()
{
}

void SystemTrayIcon::onConfigure()
{
    if (m_qmlEngine == nullptr)
    {
        auto *qmlContext = new QmlContext(this);
        auto *unityApiClient = new ucd::UnityApiClient(this);
        qmlRegisterType<ucd::Database>();
        qmlRegisterType<ucd::ProfilesModel>("ucd", 1, 0, "ProfilesModel");
        m_qmlEngine = new QQmlApplicationEngine(this);
        m_qmlEngine->rootContext()->setContextObject(qmlContext);
        m_qmlEngine->rootContext()->setContextProperty("ucdDb", m_db);
        m_qmlEngine->rootContext()->setContextProperty("unityClient", unityApiClient);
        m_qmlEngine->load(QUrl("qrc:/views/main.qml"));
        m_window = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().first());
    }
    else
    {
        m_window->show();
    }
}
