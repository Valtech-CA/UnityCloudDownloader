#include "synchronizer.h"

#include "build.h"
#include "builddao.h"
#include "buildtarget.h"
#include "buildtargetdao.h"
#include "project.h"
#include "projectdao.h"
#include "profile.h"
#include "profiledao.h"
#include "downloadsdao.h"
#include "downloadworker.h"
#include "servicelocator.h"
#include "unityapiclient.h"
#include "idatabaseprovider.h"

#include <algorithm>

#include <QThread>
#include <QtConcurrent>
#include <QDir>
#include <QDateTime>
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
    , m_workerThread(new QThread(this))
    , m_workers{}
    , m_apiClient(nullptr)
    , m_updateTimer(0)
    , m_progressTick(0)
    , m_fetchCounter(0)
{
    m_apiClient = new UnityApiClient(this);
    connect(m_apiClient, &UnityApiClient::buildsFetched, this, &Synchronizer::onBuildsFetched);
    m_updateTimer = startTimer(UpdateInterval);
    m_progressTick = startTimer(ProgressInterval);
    for (int i = 0; i < WorkerCount; ++i)
    {
        m_workers[i] = new DownloadWorker();
        m_workers[i]->moveToThread(m_workerThread);
        connect(m_workers[i], &DownloadWorker::downloadCompleted, this, &Synchronizer::onDownloadCompleted, Qt::QueuedConnection);
        connect(m_workers[i], &DownloadWorker::downloadFailed, this, &Synchronizer::onDownloadFailed, Qt::QueuedConnection);
        connect(m_workers[i], &DownloadWorker::downloadUpdated, this, &Synchronizer::onDownloadUpdated, Qt::QueuedConnection);
    }
    m_workerThread->start();

    auto downloads = DownloadsDao(ServiceLocator::database()).downloadedBuilds();
    std::sort(std::begin(downloads), std::end(downloads));
    m_downloadedBuilds = std::move(downloads);
}

Synchronizer::~Synchronizer()
{
    killTimer(m_updateTimer);
    killTimer(m_progressTick);
    m_workerThread->requestInterruption();
    m_workerThread->quit();
    if (!m_workerThread->wait(ThreadJoinTimout))
    {
        qCritical("Worker thread not ending nicely");
        m_workerThread->terminate();
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
    Build updatedBuild(build);
    updatedBuild.setManualDownload(true);
    BuildDao(ServiceLocator::database()).updateBuild(updatedBuild);
    // don't download if it is already queued or downloaded
    if (isDownloading(build) || isQueued(build) || isDownloaded(build))
        return;
    startDownload(build);
}

void Synchronizer::refresh()
{
    auto profiles = ProfileDao(ServiceLocator::database()).profiles(true);
    for (const Profile &profile : profiles)
    {
        for (const Project &project : profile.projects())
        {
            for (const BuildTarget &buildTarget : project.buildTargets())
            {
                if (buildTarget.sync())
                {
                    ++m_fetchCounter;
                    m_apiClient->fetchBuilds(buildTarget);
                }
                syncTarget(profile, project, buildTarget);
            }
        }
    }
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
        return !result;
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
        refresh();
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
    DownloadsDao(ServiceLocator::database()).addDownload(build);
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

void Synchronizer::onBuildsFetched(const QVector<Build> &builds, QUuid buildTargetId)
{
    auto checkSynchronized = [this]()
    {
        if ((--m_fetchCounter) == 0)
        {
            emit synchronized();
        }
    };

    if (builds.isEmpty())
    {
        checkSynchronized();
        auto buildTarget = BuildTargetDao(ServiceLocator::database()).buildTarget(buildTargetId);
        qWarning("fetching builds returned empty (%s)", buildTarget.name().toUtf8().data());
        return;
    }

    auto now = QDateTime::currentDateTime();
    auto db = ServiceLocator::database();
    auto buildTarget = BuildTargetDao(db).buildTarget(buildTargetId);

    if (!buildTarget.sync())
    {
        checkSynchronized();
        qInfo("synching was disabled since we requested %s", buildTarget.name().toUtf8().data());
        return;
    }

    auto project = ProjectDao(db).project(buildTarget.projectId());
    auto profile = ProfileDao(db).profile(project.profileId());
    BuildDao buildDao(db);

    int buildCount = 0;

    for (int i = 0, end = builds.size(); i < end; ++i)
    {
        const Build &build = builds.at(i);
        if (buildDao.hasBuild(build))
        {
            buildDao.partialUpdate(build);
        }
        else
        {
            buildDao.addBuild(build);
        }

        if (build.status() != Build::Status::Success || build.createTime().daysTo(now) > buildTarget.maxDaysOld())
            continue;

        BuildRef buildRef(build);
        if (!isDownloading(buildRef) && !isQueued(buildRef) && !isDownloaded(buildRef))
        {
            queueDownload(build);
        }

        if (++buildCount >= buildTarget.maxBuilds())
            break;
    }

    checkSynchronized();
}

void Synchronizer::syncTarget(const Profile &profile, const Project &project, const BuildTarget &buildTarget)
{
    QDir targetDir(QStringLiteral("%1/%2/%3").arg(profile.rootPath(), project.cloudId(), buildTarget.cloudId()));
    if (!targetDir.exists())
    {
        DownloadsDao downloadsDao(ServiceLocator::database());
        auto downloads = downloadsDao.downloadedBuilds(buildTarget.id());
        for (auto download : downloads)
        {
            downloadsDao.removeDownload(download);
            m_downloadedBuilds.removeOne(download);
        }
        return; // nothing to see there
    }

    const bool sync = buildTarget.sync();
    auto subFolders = targetDir.entryList(QDir::Dirs);
    QVector<Build> buildsToDelete;
    auto now = QDateTime::currentDateTime();
    DownloadsDao downloadsDao(ServiceLocator::database());
    BuildDao buildDao(ServiceLocator::database());
    auto downloadedBuilds = downloadsDao.downloadedBuilds(buildTarget.id());
    // sort with newer builds first
    std::sort(std::begin(downloadedBuilds), std::end(downloadedBuilds),
              [](BuildRef rhs, BuildRef lhs) -> bool { return rhs.buildNumber() > lhs.buildNumber(); });
    int itemCount = downloadedBuilds.size();
    int upCount = 0;
    for (auto buildRef : downloadedBuilds)
    {
        Build build = buildRef;
        if (!subFolders.contains(QString::number(buildRef.buildNumber())))
        {
            // folder removed, update status
            downloadsDao.removeDownload(buildRef);
            m_downloadedBuilds.removeOne(buildRef);
            --itemCount;
            // if that build was a manual download, clear the flag
            if (build.manualDownload())
            {
                build.setManualDownload(false);
                buildDao.updateBuild(build);
            }
            continue;
        }

        // if the target is not set to sync, skip the remainder
        // all we wanted to do was to update deleted builds
        if (!sync)
            continue;

        if (build.manualDownload())
        {
            ++upCount;
            continue; // manual downloads bypass garbage collect
        }

        if (++upCount > buildTarget.maxBuilds() || build.createTime().daysTo(now) > buildTarget.maxDaysOld())
        {
            buildsToDelete.append(build);
        }
    }

    // if the target is not set to sync, skip the remainder
    if (!sync)
        return;

    // sort builds to delete by oldest first
    std::sort(
                std::begin(buildsToDelete),
                std::end(buildsToDelete),
                [](const Build &rhs, const Build &lhs) -> bool
    {
        return rhs.createTime() > lhs.createTime();
    });

    for (auto build : buildsToDelete)
    {
        if (--itemCount < buildTarget.minBuilds())
            break;
        // remove old build
        downloadsDao.removeDownload(build);
        m_downloadedBuilds.removeOne(build);
        // TODO: delete in the background, better
        QDir buildDir(targetDir);
        if (buildDir.cd(QString::number(build.id())))
        {
            auto removeTask = [](QDir dir, BuildRef buildRef)
            {
                if (dir.removeRecursively())
                {
                    QUuid connectionId(QUuid::createUuid());
                    auto db = ServiceLocator::databaseProvider()->sqlDatabase(connectionId.toString());
                    db.open();
                    DownloadsDao(db).removeDownload(buildRef);
                    db.close();
                    QSqlDatabase::removeDatabase(connectionId.toString());
                }
            };
            QtConcurrent::run(removeTask, buildDir, BuildRef(build));
        }
    }
}

} // namespace ucd
