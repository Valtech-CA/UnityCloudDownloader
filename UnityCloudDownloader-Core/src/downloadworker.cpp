#include "downloadworker.h"

namespace ucd
{

DownloadWorker::DownloadWorker(QObject *parent)
    : QObject(parent)
    , m_busy(false)
{
    connect(this, &DownloadWorker::downloadRequested, this, &DownloadWorker::onDownloadRequested);
}

void DownloadWorker::download(const Build &build)
{
    m_busy = true;
    emit downloadRequested(build);
}

void DownloadWorker::onDownloadRequested(Build build)
{

}

}
