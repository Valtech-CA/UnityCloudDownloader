#include "synchronizer.h"

#include "build.h"
#include "downloadworker.h"

#include <algorithm>

#include <QThread>
#include <QtConcurrent>

#include <QDebug>

namespace ucd
{

enum
{
    ProgressInterval = 300,
    ThreadJoinTimout = 2000,
    UpdateInterval = 2 * 60 * 1000,
};

Synchronizer::Synchronizer(QObject *parent)
    : AbstractSynchronizer(parent)
    , m_workerThreads{}
    , m_workers{}
    , m_updateTimer(0)
    , m_progressTick(0)
{
    m_updateTimer = startTimer(UpdateInterval);
    m_progressTick = startTimer(ProgressInterval);
    for (int i = 0; i < WorkerCount; ++i)
    {
        m_workerThreads[i] = new QThread(this);
        m_workers[i] = new DownloadWorker();
        m_workers[i]->moveToThread(m_workerThreads[i]);
        connect(m_workers[i], &DownloadWorker::downloadCompleted, this, &Synchronizer::onDownloadCompleted, Qt::QueuedConnection);
        connect(m_workers[i], &DownloadWorker::downloadFailed, this, &Synchronizer::onDownloadFailed, Qt::QueuedConnection);
        connect(m_workers[i], &DownloadWorker::downloadUpdated, this, &Synchronizer::onDownloadUpdated, Qt::QueuedConnection);
        m_workerThreads[i]->start();
    }
}

Synchronizer::~Synchronizer()
{
    killTimer(m_updateTimer);
    killTimer(m_progressTick);
    for (QThread *thread : m_workerThreads)
    {
        thread->requestInterruption();
    }
    for (QThread *thread : m_workerThreads)
    {
        thread->quit();
        if (!thread->wait(ThreadJoinTimout))
        {
            qCritical("Worker thread not ending nicely");
            thread->terminate();
        }
    }
}

void Synchronizer::processQueue()
{
    auto workerIt = std::begin(m_workers);
    const auto workerEnd = std::end(m_workers);

    // dispatch as many downloads as possible
    for (int i = 0, end = std::min<int>(WorkerCount, m_queuedBuilds.size()); i < end; ++i)
    {
        workerIt = std::find_if(workerIt, workerEnd, [](const auto &worker) -> bool { return !worker->busy(); });
        if (workerIt == workerEnd)
            break;

        auto build = m_queuedBuilds.takeFirst();
        m_processingBuilds.append(build);
        (*workerIt)->download(build);
        emit downloadStarted(build);
    }
}

void Synchronizer::manualDownload(const Build &build)
{
    // don't download if it is already queued or downloaded
    if (isDownloading(build) || isQueued(build) || isDownloaded(build))
        return;
    startDownload(build);
}

bool Synchronizer::isQueued(const Build &build) const
{
    return m_queuedBuilds.contains(build);
}

bool Synchronizer::isDownloaded(const Build &build) const
{
    BuildRef id{build};
    bool result = std::binary_search(
                std::begin(m_downloadedBuilds),
                std::end(m_downloadedBuilds),
                id);

    if (result != m_downloadedBuilds.contains(build))
    {
        qCritical("missmatch in binary search vs contains");
        qDebug() << m_downloadedBuilds;
    }

    return result;
}

bool Synchronizer::isDownloading(const Build &build) const
{
    return m_processingBuilds.contains(build);
}

float Synchronizer::downloadProgress(const Build &build) const
{
    BuildRef id{build};
    if (!isDownloading(id) || !m_downloadStats.contains(id))
        return 0;
    return m_downloadStats[id].first;
}

qint64 Synchronizer::downloadSpeed(const Build &build) const
{
    BuildRef id{build};
    if (!isDownloading(id) || !m_downloadStats.contains(id))
        return 0;
    return m_downloadStats[id].second;
}

void Synchronizer::queueDownload(const Build &build)
{
    m_queuedBuilds.append(build);
    emit downloadQueued(build);
    processQueue();
}

void Synchronizer::startDownload(const Build &build)
{
    // we don't actually start the download right away, instead we insert it at the begining of the queue
    m_queuedBuilds.prepend(build);
    emit downloadQueued(build);
    processQueue();
}

void Synchronizer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateTimer)
    {
        // TODO: compute changes and queue new downloads
    }
    else if (event->timerId() == m_progressTick)
    {
        for (auto *worker : m_workers)
        {
            worker->requestProgress();
        }
    }
}

void Synchronizer::onDownloadCompleted(Build build)
{
    if (!m_processingBuilds.removeOne(build))
    {
        qCritical("could not remove processing build on completed");
    }

    auto insertIt = std::lower_bound(
                std::begin(m_downloadedBuilds),
                std::end(m_downloadedBuilds),
                build);
    m_downloadedBuilds.insert(insertIt, build);
    m_downloadStats.remove(build);
    emit downloadCompleted(build);
    processQueue();
}

void Synchronizer::onDownloadFailed(Build build)
{
    if (!m_processingBuilds.removeOne(build))
    {
        qCritical("could not remove processing build on failure");
    }

    m_queuedBuilds.prepend(build);
    m_downloadStats.remove(build);
    emit downloadFailed(build);
    processQueue();
}

void Synchronizer::onDownloadUpdated(Build build, float ratio, qint64 speed)
{
    m_downloadStats[build] = qMakePair(ratio, speed);
    emit downloadUpdated(build);
}

} // namespace ucd
