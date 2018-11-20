#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

namespace ucd
{
class Database;
}

class QAction;
class QQmlApplicationEngine;
class QQuickWindow;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(ucd::Database *data, QObject *parent = nullptr);
    virtual ~SystemTrayIcon() override = default;

private slots:
    void onConfigure();

private:
    ucd::Database *m_db;
    QMenu *m_menu;
    QAction *m_quitAction;
    QQmlApplicationEngine *m_qmlEngine;
    QQuickWindow *m_window;
};

#endif // SYSTEMTRAYICON_H
