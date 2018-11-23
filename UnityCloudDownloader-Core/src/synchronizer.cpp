#include "synchronizer.h"

#include "build.h"
#include "downloadworker.h"

#include <algorithm>

#include <QThread>
#include <QtConcurrent>

namespace ucd
{

enum
{
    ThreadJoinTimout = 2000
};

Synchronizer::Synchronizer(QObject *parent)
    : AbstractSynchronizer(parent)
    , m_workerThreads{}
    , m_workers{}
{
    for (int i = 0; i < WorkerCount; ++i)
    {
        m_workerThreads[i] = new QThread(this);
        m_workers[i] = new DownloadWorker();
        m_workers[i]->moveToThread(m_workerThreads[i]);
        connect(m_workers[i], &DownloadWorker::downloadCompleted, this, &Synchronizer::onDownloadCompleted, Qt::QueuedConnection);
    }
}

Synchronizer::~Synchronizer()
{
    for (QThread *thread : m_workerThreads)
    {
        thread->requestInterruption();
    }
    for (QThread *thread : m_workerThreads)
    {
        thread->quit();
        if (!thread->wait(ThreadJoinTimout))
        {
            qCritical("Worker thread not ending nicely;");
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
    }
}

void Synchronizer::manualDownload(const Build &build)
{
    // don't download if it is already queued or downloaded
    if (isQueued(build) || isDownloaded(build))
        return;
    startDownload(build);
}

bool Synchronizer::isQueued(const Build &build) const
{
    BuildRef id{build};
    return std::binary_search(std::begin(m_processingBuilds),
                              std::end(m_processingBuilds),
                              id)
                    || std::binary_search(
                            std::begin(m_queuedBuilds),
                            std::end(m_queuedBuilds),
                            id);
}

bool Synchronizer::isDownloaded(const Build &build) const
{
    BuildRef id{build};
    return std::binary_search(
                std::begin(m_downloadedBuilds),
                std::end(m_downloadedBuilds),
                id);
}

void Synchronizer::queueDownload(const Build &build)
{
    m_queuedBuilds.append(build);
    processQueue();
}

void Synchronizer::startDownload(const Build &build)
{
    // we don't actually start the download right away, instead we insert it at the begining of the queue
    m_queuedBuilds.prepend(build);
    processQueue();
}

void Synchronizer::onDownloadCompleted(Build build)
{
    m_processingBuilds.removeOne(build);
    auto insertIt = std::lower_bound(
                std::begin(m_downloadedBuilds),
                std::end(m_downloadedBuilds),
                build);
    m_downloadedBuilds.insert(insertIt, build);
}

} // namespace ucd
