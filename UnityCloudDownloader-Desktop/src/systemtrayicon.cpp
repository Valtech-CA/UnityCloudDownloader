#include "systemtrayicon.h"

#include "unityapiclient.h"
#include "profilemodel.h"

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>

SystemTrayIcon::SystemTrayIcon(ucd::Database *data, QObject *parent)
    : QSystemTrayIcon(parent)
    , m_db(data)
    , m_menu(nullptr)
    , m_quitAction(nullptr)
    , m_qmlEngine(nullptr)
    , m_view(nullptr)
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
    if (m_view != nullptr)
    {
        m_view->destroy();
        m_view = nullptr;
    }
}

void SystemTrayIcon::onConfigure()
{
    if (m_qmlEngine == nullptr)
    {
        auto *unityApiClient = new ucd::UnityApiClient(this);
        auto *profileModel = new ucd::ProfilesModel(m_db, this);
        m_qmlEngine = new QQmlApplicationEngine(this);
        m_qmlEngine->rootContext()->setContextProperty("unityClient", unityApiClient);
        m_qmlEngine->rootContext()->setContextProperty("profileModel", profileModel);
        m_view = new QQuickView(m_qmlEngine, nullptr);
        m_view->setTitle(tr("Unity Cloud Downloader"));
        m_view->setSource(QUrl("qrc:/views/main.qml"));
        m_view->setWidth(600);
        m_view->setHeight(800);
        m_view->show();
    }
    else
    {
        m_view->show();
    }
}
