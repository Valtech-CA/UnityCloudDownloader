#ifndef UCD_SYNCHRONIZER_H
#define UCD_SYNCHRONIZER_H

#include "abstractsynchronizer.h"
#include "build.h"

#include <QVector>

class QThread;

namespace ucd
{

class Database;
class BuildRef;
class DownloadWorker;

class Synchronizer : public AbstractSynchronizer
{
    enum
    {
        WorkerCount = 3
    };
    Q_OBJECT
public:
    Synchronizer(QObject *parent = nullptr);
    ~Synchronizer() override;

    void processQueue();
    void manualDownload(const Build &build) override;

    bool isQueued(const Build &build) const;
    bool isDownloaded(const Build &build) const;

    void queueDownload(const Build &build);
    void startDownload(const Build &build);

private slots:
    void onDownloadCompleted(Build build);

private:
    QVector<BuildRef> m_processingBuilds;
    QVector<BuildRef> m_queuedBuilds;
    QVector<BuildRef> m_downloadedBuilds;
    QThread *m_workerThreads[WorkerCount];
    DownloadWorker *m_workers[WorkerCount];
};

}

#endif // UCD_SYNCHRONIZER_H
