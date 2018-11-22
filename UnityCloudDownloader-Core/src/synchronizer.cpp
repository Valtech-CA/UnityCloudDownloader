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


}

void Synchronizer::manualDownload(const Build &build)
{
    // don't download if it is already queued or downloaded
    if (isQueued(build) || isDownloaded(build))
        return;

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



} // namespace ucd
