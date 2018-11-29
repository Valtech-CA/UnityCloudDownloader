#include "runguard.h"

#include <QSharedMemory>
#include <QStandardPaths>
#include <QDir>
#include <QTimerEvent>
#include <QCryptographicHash>

// see https://stackoverflow.com/questions/5006547/qt-best-practice-for-a-single-instance-app-protection

namespace
{
enum
{
    TryLockTimeout = 150,
    ReadInterval = 200,
    SharedSize = 64,
    MagickMarker = 0xC0FFEE,
};

QString generateKeyHash(const QString &key, const QString &salt)
{
    QByteArray data;
    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
    return data;
}
}

//static QString lockFileName()
//{
//    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)).filePath("app.lock");
//}

RunGuard::RunGuard(const QString &key, QObject *parent)
    : QObject(parent)
    , m_key(key)
    , m_lockKey(generateKeyHash(key, QStringLiteral("_lockKey")))
    , m_sharedKey(generateKeyHash(key, QStringLiteral("_sharedKey")))
    , m_semaphore(m_lockKey, 1)
    , m_mem(nullptr)
    , m_timerId(0)
{
    m_mem = new QSharedMemory(m_sharedKey, this);
#ifndef Q_OS_WIN
    m_semaphore.acquire();
    {
        // on linux, this will clear zombied shared memory after an application crash
        QSharedMemory detachZombie(m_sharedKey);
        detachZombie.attach();
    }
#endif
}

RunGuard::~RunGuard()
{
    unlock();
}

bool RunGuard::isAnotherRunning()
{
    if (m_mem->isAttached())
        return false;

    m_semaphore.acquire();
    const bool isRunning = m_mem->attach();
    if (isRunning)
        m_mem->detach();
    m_semaphore.release();

    return isRunning;
}

bool RunGuard::tryLock()
{
    if (m_mem->isAttached())
        return false;

    m_semaphore.acquire();
    const bool result = m_mem->create(SharedSize);
    m_semaphore.release();

    if (!result)
    {
        if (m_mem->attach() && m_mem->lock())
        {
            int *data = reinterpret_cast<int*>(m_mem->data());
            data[0] = MagickMarker;
            m_mem->unlock();
        }
        m_mem->detach();
        return false;
    }

    m_timerId = startTimer(ReadInterval);
    return true;
}

void RunGuard::startWatch()
{
    if (m_timerId == 0)
    {
        m_timerId = startTimer(ReadInterval);
    }
}

void RunGuard::unlock()
{
    m_semaphore.acquire();
    if (m_mem->isAttached())
        m_mem->detach();
    m_semaphore.release();

    if (m_timerId != 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}

void RunGuard::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId && m_mem->isAttached())
    {
        // read shared memory to check if someone else tried to start an other instance
        if (!m_mem->lock())
        {
            qFatal("cannot lock shared memory");
        }
        int *data = reinterpret_cast<int*>(m_mem->data());
        if (data == nullptr)
        {
            qFatal("shared memory returned a null pointer");
        }
        const bool hasTrigger = MagickMarker == data[0];
        data[0] = 0;
        m_mem->unlock();
        if (hasTrigger)
        {
            emit triggered();
        }
    }
}
