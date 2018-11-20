#include "buildtarget.h"

#include <QDataStream>

namespace ucd
{

BuildTarget::BuildTarget()
    : m_id(QUuid::createUuid())
    , m_sync(false)
    , m_minBuilds(1)
    , m_maxBuilds(10)
    , m_maxDaysOld(30)
{}

BuildTarget::BuildTarget(const BuildTarget &other)
    : m_id(other.m_id)
    , m_projectId(other.m_projectId)
    , m_cloudId(other.m_cloudId)
    , m_name(other.m_name)
    , m_platform(other.m_platform)
    , m_sync(other.m_sync)
    , m_minBuilds(other.m_minBuilds)
    , m_maxBuilds(other.m_maxBuilds)
    , m_maxDaysOld(other.m_maxDaysOld)
{}

BuildTarget::BuildTarget(BuildTarget &&other)
    : m_id(std::move(other.m_id))
    , m_projectId(std::move(other.m_projectId))
    , m_cloudId(std::move(other.m_cloudId))
    , m_name(std::move(other.m_name))
    , m_platform(std::move(other.m_platform))
    , m_sync(other.m_sync)
    , m_minBuilds(other.m_minBuilds)
    , m_maxBuilds(other.m_maxBuilds)
    , m_maxDaysOld(other.m_maxDaysOld)
{}

BuildTarget::~BuildTarget()
{}

BuildTarget &BuildTarget::operator=(const BuildTarget &other)
{
    this->~BuildTarget();
    new (this) BuildTarget(other);
    return *this;
}

BuildTarget &BuildTarget::operator=(BuildTarget &&other)
{
    this->~BuildTarget();
    new (this) BuildTarget(std::move(other));
    return *this;
}

void BuildTarget::setName(const QString &name)
{
    m_name = name;
}

void BuildTarget::setId(const QUuid &id)
{
    m_id = id;
}

void BuildTarget::setProjectId(const QUuid &projectId)
{
    m_projectId = projectId;
}

void BuildTarget::setCloudId(const QString &cloudId)
{
    m_cloudId = cloudId;
}

void BuildTarget::setPlatform(const QString &platform)
{
    m_platform = platform;
}

void BuildTarget::setSync(bool value)
{
    m_sync = value;
}

void BuildTarget::setMinBuilds(int value)
{
    m_minBuilds = value;
}

void BuildTarget::setMaxBuilds(int value)
{
    m_maxBuilds = value;
}

void BuildTarget::setMaxDaysOld(int value)
{
    m_maxDaysOld = value;
}

} // namespace ucd

QDataStream &operator<<(QDataStream &out, const ucd::BuildTarget &value)
{
    out << value.name() << value.id() << value.projectId() << value.cloudId() << value.platform()
        << value.sync() << value.minBuilds() << value.maxBuilds() << value.maxDaysOld();
    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::BuildTarget &dest)
{
    QString name;
    in >> name;
    QUuid id;
    in >> id;
    QUuid projectId;
    in >> projectId;
    QString cloudId;
    in >> cloudId;
    QString platform;
    in >> platform;
    bool sync;
    in >> sync;
    int minBuilds;
    in >> minBuilds;
    int maxBuilds;
    in >> maxBuilds;
    int maxDaysOld;
    in >> maxDaysOld;

    dest.setName(name);
    dest.setId(id);
    dest.setProjectId(projectId);
    dest.setCloudId(cloudId);
    dest.setPlatform(platform);
    dest.setSync(sync);
    dest.setMinBuilds(minBuilds);
    dest.setMaxBuilds(maxBuilds);
    dest.setMaxDaysOld(maxDaysOld);

    return in;
}
