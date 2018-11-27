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
#include <QProcess>

namespace ucd
{

enum
{
    BufferReserve = 15000,
};

DownloadWorker::DownloadWorker(QObject *parent)
    : QObject(parent)
    , m_connectionId(QUuid::createUuid())
    , m_busy(false)
    , m_network(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_lastSize(0)
{
    m_buffer.reserve(BufferReserve);
    connect(this, &DownloadWorker::downloadRequested, this, &DownloadWorker::onDownloadRequested, Qt::QueuedConnection);
    connect(this, &DownloadWorker::progressRequested, this, &DownloadWorker::onProgressRequested, Qt::QueuedConnection);
}

DownloadWorker::~DownloadWorker()
{}

void DownloadWorker::download(const Build &build)
{
    m_busy = true;
    emit downloadRequested(build);
}

void DownloadWorker::requestProgress()
{
    if (m_busy)
        emit progressRequested();
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
    m_filePath = storageDir.filePath(build.artifactName());
    m_outFile = std::make_unique<QFile>(m_filePath);
    if (!m_outFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        m_outFile = nullptr;
        qCritical("Cannot create file %s for writing", m_filePath.toUtf8().data());
        emit downloadFailed(build);
        return;
    }

    // start download
    QNetworkRequest request(build.artifactPath());
    m_reply = m_network->get(request);
    connect(m_reply, &QNetworkReply::readyRead, this, &DownloadWorker::onReadyRead);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadWorker::onDownloadFinished);
    connect(m_reply, &QNetworkReply::finished, m_reply, &QNetworkReply::deleteLater);
    m_progressTimer.start();
    m_lastSize = 0;
}

void DownloadWorker::onReadyRead()
{
    qint64 bytesRead = 0;
    while ((bytesRead = m_reply->read(m_buffer.data(), m_buffer.capacity())) > 0)
    {
        m_outFile->write(m_buffer.data(), bytesRead);
    }
}

void DownloadWorker::onDownloadFinished()
{
    emit downloadUpdated(m_build, 1, 0);
    m_progressTimer.invalidate();
    QSqlDatabase::removeDatabase(m_connectionId.toString());
    m_outFile->close();
    m_outFile = nullptr;

    if (m_reply->error())
    {
        qCritical("Download failed %s", m_reply->errorString().toUtf8().data());
        m_reply = nullptr;
        m_busy = false;
        emit downloadFailed(m_build);
    }
    else
    {
        m_reply = nullptr;
#ifdef Q_OS_WIN
        auto *unzip = new QProcess(this);
        QStringList args{
            QStringLiteral("-nologo"),
            QStringLiteral("-noprofile"),
            QStringLiteral("-command"),
            QStringLiteral("& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('%1', '%2'); }")
            .arg(m_filePath, m_storagePath)
        };
        unzip->setProgram(QStringLiteral("powershell.exe"));
        unzip->setArguments(args);
        void (QProcess::*finished)(int) = &QProcess::finished; // finished is overloaded which prevents auto selection
        connect(unzip, finished, this, &DownloadWorker::onUnzipFinished);
        connect(unzip, finished, unzip, &QProcess::deleteLater);
        unzip->start();
#else
        m_busy = false;
        emit downloadCompleted(m_build);
#endif
    }
}

void DownloadWorker::onProgressRequested()
{
    if (m_reply == nullptr)
        return;

    qint64 currentSize = m_outFile->size();
    float ratio = float(currentSize) / m_build.artifactSize();
    // calculate the speed in bytes per second
    auto elapsedTime = m_progressTimer.restart();
    qint64 speed = (elapsedTime != 0) ? (((currentSize - m_lastSize) * 1000) / elapsedTime) : 0;
    m_lastSize = currentSize;

    emit downloadUpdated(m_build, ratio, speed);
}

void DownloadWorker::onUnzipFinished(int exitCode)
{
    if (exitCode == 0)
    {
        QFile::remove(m_filePath);
        m_busy = false;
        emit downloadCompleted(m_build);
    }
    else
    {
        qCritical("unzip failed");
        m_busy = false;
        emit downloadFailed(m_build);
    }
}

}
