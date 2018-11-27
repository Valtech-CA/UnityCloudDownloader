#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include "build.h"

#include <QSystemTrayIcon>

class QAction;
class QQmlApplicationEngine;
class QQuickWindow;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    virtual ~SystemTrayIcon() override = default;

    void configure();

private slots:
    void onConfigure();
    void onRefresh();
    void onDownloadCompleted(ucd::Build build);
    void onMessageClicked();

private:
    QMenu *m_menu;
    QAction *m_quitAction;
    QQmlApplicationEngine *m_qmlEngine;
    QQuickWindow *m_window;
    ucd::BuildRef m_lastBuildDownloaded;
};

#endif // SYSTEMTRAYICON_H
