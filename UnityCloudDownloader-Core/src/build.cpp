#include "build.h"

namespace ucd
{

Build::Status Build::statusFromString(const QString &value)
{
    if (value == QStringLiteral("queued"))
        return Status::Queued;
    if (value == QStringLiteral("sentToBuilder"))
        return Status::SentToBuilder;
    if (value == QStringLiteral("starter"))
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

void Build::setIconPath(QString iconPath)
{
    m_iconPath = std::move(iconPath);
}

void Build::setArtifactName(QString artifactName)
{
    m_artifactName = std::move(artifactName);
}

void Build::setArtifactSize(uint64_t size)
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
