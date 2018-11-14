#include "buildtarget.h"

#include <QDataStream>

namespace ucd
{

BuildTarget::BuildTarget()
{}

BuildTarget::BuildTarget(const BuildTarget &other)
    : m_id(other.m_id)
    , m_profileId(other.m_profileId)
    , m_projectId(other.m_projectId)
    , m_name(other.m_name)
    , m_platform(other.m_platform)
{}

BuildTarget::BuildTarget(BuildTarget &&other)
    : m_id(std::move(other.m_id))
    , m_profileId(std::move(other.m_profileId))
    , m_projectId(std::move(other.m_projectId))
    , m_name(std::move(other.m_name))
    , m_platform(std::move(other.m_platform))
{}

BuildTarget::~BuildTarget()
{}

BuildTarget &BuildTarget::operator=(const BuildTarget &other)
{
    new (this) BuildTarget(other);
    return *this;
}

BuildTarget &BuildTarget::operator=(BuildTarget &&other)
{
    new (this) BuildTarget(std::move(other));
    return *this;
}

void BuildTarget::setName(const QString &name)
{
    m_name = name;
}

void BuildTarget::setId(const QString &id)
{
    m_id = id;
}

void BuildTarget::setProfileId(const QString &profileId)
{
    m_profileId = profileId;
}

void BuildTarget::setProjectId(const QString &projectId)
{
    m_projectId = projectId;
}

void BuildTarget::setPlatform(const QString &platform)
{
    m_platform = platform;
}

} // namespace ucd

QDataStream &operator<<(QDataStream &out, const ucd::BuildTarget &value)
{
    out << value.name() << value.id() << value.profileId() << value.projectId() << value.platform();
    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::BuildTarget &dest)
{
    QString name;
    in >> name;
    QString id;
    in >> id;
    QString profileId;
    in >> profileId;
    QString projectId;
    in >> projectId;
    QString platform;
    in >> platform;

    dest.setName(name);
    dest.setId(id);
    dest.setProfileId(profileId);
    dest.setProjectId(projectId);
    dest.setPlatform(platform);

    return in;
}
