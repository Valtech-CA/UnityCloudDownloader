#ifndef RUNGUARD_H
#define RUNGUARD_H

#pragma once

#include <QObject>
#include <QSystemSemaphore>

class QSharedMemory;

class RunGuard : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RunGuard)
public:
    explicit RunGuard(const QString &key, QObject *parent = nullptr);
    ~RunGuard() override;

    bool isAnotherRunning();
    bool tryLock();

    void startWatch();

    void unlock();

signals:
    void triggered();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    QString m_key;
    QString m_lockKey;
    QString m_sharedKey;
    QSystemSemaphore m_semaphore;
    QSharedMemory *m_mem;
    int m_timerId;
};

#endif // RUNGUARD_H
