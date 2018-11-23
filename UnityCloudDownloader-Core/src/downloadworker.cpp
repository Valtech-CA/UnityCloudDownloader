#include "downloadworker.h"

#include "servicelocator.h"
#include "idatabaseprovider.h"
#include "profile.h"
#include "profiledao.h"
#include "project.h"
#include "projectdao.h"
#include "buildtarget.h"
#include "buildtargetdao.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QFile>

namespace ucd
{

enum
{
    BufferReserve = 5000
};

DownloadWorker::DownloadWorker(QObject *parent)
    : QObject(parent)
    , m_connectionId(QUuid::createUuid())
    , m_busy(false)
    , m_network(new QNetworkAccessManager(this))
{
    m_buffer.reserve(BufferReserve);
    connect(this, &DownloadWorker::downloadRequested, this, &DownloadWorker::onDownloadRequested);
}

DownloadWorker::~DownloadWorker()
{}

void DownloadWorker::download(const Build &build)
{
    m_busy = true;
    emit downloadRequested(build);
}

void DownloadWorker::onDownloadRequested(Build build)
{
    m_build = build;

    // setup storage path
    QSqlDatabase db = ServiceLocator::databaseProvider()->sqlDatabase(m_connectionId.toString());
    if (!db.open())
    {
        qCritical("Cannot open dabatase connection");
        emit downloadFailed(build);
        return;
    }
    auto buildTarget = BuildTargetDao(db).buildTarget(build.buildTargetId());
    auto project = ProjectDao(db).project(buildTarget.projectId());
    auto profile = ProfileDao(db).profile(project.profileId());
    db.close();

    auto storageDir = QDir(QStringLiteral("%1/%2/%3/%4").arg(
                               profile.rootPath(),
                               project.cloudId(),
                               buildTarget.cloudId(),
                               QString::number(build.id())));
    m_storagePath = storageDir.absolutePath();
    storageDir.mkpath(m_storagePath);
    auto filePath = storageDir.filePath(build.artifactName());
    m_outFile = std::make_unique<QFile>(filePath);
    if (!m_outFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        m_outFile = nullptr;
        qCritical("Cannot create file %s for writing", filePath.toUtf8().data());
        emit downloadFailed(build);
        return;
    }

    // start download
    QNetworkRequest request(build.artifactPath());
    auto *reply = m_network->get(request);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadWorker::onReadyRead);
    connect(reply, &QNetworkReply::finished, this, &DownloadWorker::onDownloadFinished);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void DownloadWorker::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    qint64 bytesRead = 0;
    while ((bytesRead = reply->read(m_buffer.data(), m_buffer.capacity())) > 0)
    {
        m_outFile->write(m_buffer.data(), bytesRead);
    }
}

void DownloadWorker::onDownloadFinished()
{
    QSqlDatabase::removeDatabase(m_connectionId.toString());
    m_outFile->close();
    m_outFile = nullptr;

    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
    {
        qCritical("Download failed %s", reply->errorString().toUtf8().data());
        emit downloadFailed(m_build);
    }
    else
    {
        m_busy = false;
        emit downloadCompleted(m_build);
    }
}

}
