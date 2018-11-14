#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class QAction;
class QQmlApplicationEngine;
class QQuickView;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    ~SystemTrayIcon();

private slots:
    void onConfigure();

private:
    QMenu *m_menu;
    QAction *m_quitAction;
    QQmlApplicationEngine *m_qmlEngine;
    QQuickView *m_view;
};

#endif // SYSTEMTRAYICON_H
