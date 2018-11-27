#include "systemtrayicon.h"

#include "qmlcontext.h"

#include "servicelocator.h"
#include "abstractsynchronizer.h"
#include "unityapiclient.h"
#include "profilesmodel.h"
#include "projectsmodel.h"
#include "buildtargetsmodel.h"
#include "build.h"
#include "buildsmodel.h"

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QFileInfo>
#include <QDesktopServices>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_menu(nullptr)
    , m_quitAction(nullptr)
    , m_qmlEngine(nullptr)
    , m_window(nullptr)
{
    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);

    auto configureAction = new QAction(tr("&Configure"), this);
    connect(configureAction, &QAction::triggered, this, &SystemTrayIcon::onConfigure);

    auto refreshAction = new QAction(tr("&Refresh"), this);
    connect(refreshAction, &QAction::triggered, this, &SystemTrayIcon::onRefresh);

    m_menu = new QMenu();
    m_menu->addAction(configureAction);
    m_menu->addAction(refreshAction);
    m_menu->addSeparator();
    m_menu->addAction(m_quitAction);

    setContextMenu(m_menu);
    setToolTip(tr("Unity Cloud Downloader"));
    setIcon(QIcon(":/icons/UnityCloudLogo2.png"));

    auto synchronizer = ucd::ServiceLocator::synchronizer();
    connect(synchronizer, &ucd::AbstractSynchronizer::downloadCompleted, this, &SystemTrayIcon::onDownloadCompleted);

    connect(this, &QSystemTrayIcon::messageClicked, this, &SystemTrayIcon::onMessageClicked);
}

void SystemTrayIcon::onConfigure()
{
    if (m_qmlEngine == nullptr)
    {
        auto *qmlContext = new QmlContext(this);
        auto *unityApiClient = new ucd::UnityApiClient(this);
        qmlRegisterType<ucd::ProfilesModel>("ucd", 1, 0, "ProfilesModel");
        qmlRegisterType<ucd::ProjectsModel>("ucd", 1, 0, "ProjectsModel");
        qmlRegisterType<ucd::BuildTargetsModel>("ucd", 1, 0, "BuildTargetsModel");
        qmlRegisterType<ucd::BuildsModel>("ucd", 1, 0, "BuildsModel");
        qmlRegisterUncreatableMetaObject(ucd::Build::staticMetaObject, "ucd", 1, 0, "Build", "Cannot create Build object");
        m_qmlEngine = new QQmlApplicationEngine(this);
        m_qmlEngine->rootContext()->setContextObject(qmlContext);
        m_qmlEngine->rootContext()->setContextProperty("unityClient", unityApiClient);
        m_qmlEngine->load(QUrl("qrc:/views/main.qml"));
        m_window = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().first());
    }
    else
    {
        m_window->show();
    }
}

void SystemTrayIcon::onRefresh()
{
    ucd::ServiceLocator::synchronizer()->refresh();
}

void SystemTrayIcon::onDownloadCompleted(ucd::Build build)
{
    m_lastBuildDownloaded = build;
    showMessage(tr("%1 build %2 downloaded").arg(build.name(), QString::number(build.id())), build.downloadFolderPath());
}

void SystemTrayIcon::onMessageClicked()
{
    auto dirPath = ucd::Build(m_lastBuildDownloaded).downloadFolderPath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}
