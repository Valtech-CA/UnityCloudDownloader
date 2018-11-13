#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class QAction;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    ~SystemTrayIcon();

private slots:

private:
    QMenu *m_menu;
    QAction *m_quitAction;
};

#endif // SYSTEMTRAYICON_H
