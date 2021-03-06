#include "build.h"

#include "buildtarget.h"
#include "buildtargetdao.h"
#include "project.h"
#include "projectdao.h"
#include "profile.h"
#include "profiledao.h"
#include "servicelocator.h"

#include <QSqlDatabase>
#include <QDataStream>

namespace ucd
{

Build::Status Build::statusFromString(const QString &value)
{
    if (value == QStringLiteral("queued"))
        return Status::Queued;
    if (value == QStringLiteral("sentToBuilder"))
        return Status::SentToBuilder;
    if (value == QStringLiteral("started"))
        return Status::Started;
    if (value == QStringLiteral("restarted"))
        return Status::Restarted;
    if (value == QStringLiteral("success"))
        return Status::Success;
    if (value == QStringLiteral("failure"))
        return Status::Failure;
    if (value == QStringLiteral("canceled"))
        return Status::Canceled;
    return Status::Unknown;
}

Build::Build()
    : m_buildNumber(0)
    , m_status(Status::Unknown)
    , m_artifactSize(0)
    , m_manual(false)
{

}

bool Build::isLike(const Build &other)
{
    return m_name == other.m_name
            && m_buildNumber == other.m_buildNumber
            && m_status == other.m_status
            && m_iconPath == other.m_iconPath
            && m_artifactName == other.m_artifactName
            && m_artifactSize == other.m_artifactSize
            && m_artifactPath == other.m_artifactPath;
}

void Build::takeFrom(const Build &other)
{
    // copy everything but the build target id
    m_name = other.m_name;
    m_buildNumber = other.m_buildNumber;
    m_status = other.m_status;
    m_iconPath = other.m_iconPath;
    m_artifactName = other.m_artifactName;
    m_artifactSize = other.m_artifactSize;
    m_artifactPath = other.m_artifactPath;
}

QString Build::downloadFolderPath() const
{
    auto db = ServiceLocator::database();
    auto target = BuildTargetDao(db).buildTarget(m_buildTargetId);
    auto project = ProjectDao(db).project(target.projectId());
    auto profile = ProfileDao(db).profile(project.profileId());
    return QStringLiteral("%1/%2/%3/%4").arg(
                profile.rootPath(),
                project.cloudId(),
                target.cloudId(),
                QString::number(m_buildNumber));
}

QString Build::downloadFilePath() const
{
    auto db = ServiceLocator::database();
    auto target = BuildTargetDao(db).buildTarget(m_buildTargetId);
    auto project = ProjectDao(db).project(target.projectId());
    auto profile = ProfileDao(db).profile(project.profileId());
    return QStringLiteral("%1/%2/%3/%4/%5").arg(
                profile.rootPath(),
                project.cloudId(),
                target.cloudId(),
                QString::number(m_buildNumber),
                m_artifactName);
}

void Build::setName(QString name)
{
    m_name = std::move(name);
}

void Build::setId(int id)
{
    m_buildNumber = id;
}

void Build::setStatus(int status)
{
    m_status = static_cast<Status>(status);
}

void Build::setBuildTargetId(const QUuid &buildTargetId)
{
    m_buildTargetId = buildTargetId;
}

void Build::setCreateTime(const QDateTime &createTime)
{
    m_createTime = createTime;
}

void Build::setIconPath(QString iconPath)
{
    m_iconPath = std::move(iconPath);
}

void Build::setArtifactName(QString artifactName)
{
    m_artifactName = std::move(artifactName);
}

void Build::setArtifactSize(qint64 size)
{
    m_artifactSize = size;
}

void Build::setArtifactPath(QString artifactPath)
{
    m_artifactPath = std::move(artifactPath);
}

void Build::setManualDownload(bool value)
{
    m_manual = value;
}

} // namespace ucd

QDataStream &operator<<(QDataStream &out, const ucd::Build &value)
{
    out
            << value.id()
            << value.name()
            << value.buildTargetId()
            << static_cast<int>(value.status())
            << value.iconPath()
            << value.artifactName()
            << value.artifactSize()
            << value.artifactPath();

    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::Build &dest)
{
    int buildNumber;
    in >> buildNumber;
    QString name;
    in >> name;
    QUuid buildTargetId;
    in >> buildTargetId;
    int status;
    in >> status;
    QString iconPath;
    in >> iconPath;
    QString artifactName;
    in >> artifactName;
    qint64 artifactSize;
    in >> artifactSize;
    QString artifactPath;
    in >> artifactPath;

    dest.setId(buildNumber);
    dest.setName(name);
    dest.setBuildTargetId(buildTargetId);
    dest.setStatus(status);
    dest.setIconPath(iconPath);
    dest.setArtifactName(artifactName);
    dest.setArtifactSize(artifactSize);
    dest.setArtifactPath(artifactPath);

    return in;
}
