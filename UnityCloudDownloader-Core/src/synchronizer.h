#ifndef UCD_SYNCHRONIZER_H
#define UCD_SYNCHRONIZER_H

#include "abstractsynchronizer.h"

#include <QVector>
#include <QMap>
#include <QPair>

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

    bool isQueued(const Build &build) const override;
    bool isDownloaded(const Build &build) const override;
    bool isDownloading(const Build &build) const override;
    float downloadProgress(const Build &build) const override;
    qint64 downloadSpeed(const Build &build) const override;

    void queueDownload(const Build &build);
    void startDownload(const Build &build);

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void onDownloadCompleted(ucd::Build build);
    void onDownloadFailed(ucd::Build build);
    void onDownloadUpdated(ucd::Build build, float ratio, qint64 speed);

private:
    QVector<BuildRef> m_processingBuilds;
    QVector<BuildRef> m_queuedBuilds;
    QVector<BuildRef> m_downloadedBuilds;
    QThread *m_workerThreads[WorkerCount];
    DownloadWorker *m_workers[WorkerCount];
    QMap<BuildRef, QPair<float, qint64>> m_downloadStats;
    int m_updateTimer;
    int m_progressTick;
};

}

#endif // UCD_SYNCHRONIZER_H
