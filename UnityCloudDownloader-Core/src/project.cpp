#include "project.h"

#include <QDataStream>

namespace ucd
{

Project::Project()
    : m_id(QUuid::createUuid())
{}

Project::Project(Project &&other) noexcept
    : m_id(other.m_id)
    , m_profileId(other.m_profileId)
    , m_cloudId(std::move(other.m_cloudId))
    , m_name(std::move(other.m_name))
    , m_orgId(std::move(other.m_orgId))
    , m_iconPath(std::move(other.m_iconPath))
    , m_buildTargets(std::move(other.m_buildTargets))
{}

Project &Project::operator=(const Project &other)
{
    this->~Project();
    new (this) Project(other);
    return *this;
}

Project &Project::operator=(Project &&other) noexcept
{
    this->~Project();
    new (this) Project(std::move(other));
    return *this;
}

void Project::setName(const QString &name)
{
    m_name = name;
}

void Project::setId(const QUuid &id)
{
    m_id = id;
}

void Project::setProfileId(const QUuid &profileId)
{
    m_profileId = profileId;
}

void Project::setCloudId(const QString &cloudId)
{
    m_cloudId = cloudId;
}

void Project::setOrganisationId(const QString &id)
{
    m_orgId = id;
}

void Project::setIconPath(const QString &iconPath)
{
    m_iconPath = iconPath;
}

void Project::setBuildTargets(const BuildTargetList &buildTargets)
{
    m_buildTargets = buildTargets;
}

} // namespace ucd

QDataStream &operator<<(QDataStream &out, const ucd::BuildTargetList &value)
{
    out << value.size();
    for (const auto &item : value)
    {
        out << item;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::BuildTargetList &dest)
{
    int length;
    in >> length;
    dest.reserve(length);
    ucd::BuildTarget item;
    for (int i = 0; i < length; ++i)
    {
        in >> item;
        dest.append(std::move(item));
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const ucd::Project &value)
{
    out << value.name() << value.id() << value.profileId() << value.cloudId() << value.organisationId() << value.iconPath() << value.buildTargets();
    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::Project &dest)
{
    QString name;
    in >> name;
    QUuid id;
    in >> id;
    QUuid profileId;
    in >> profileId;
    QString cloudId;
    in >> cloudId;
    QString orgId;
    in >> orgId;
    QString iconPath;
    in >> iconPath;
    ucd::BuildTargetList buildTargetList;
    in >> buildTargetList;

    dest.setName(name);
    dest.setId(id);
    dest.setProfileId(profileId);
    dest.setCloudId(cloudId);
    dest.setOrganisationId(orgId);
    dest.setIconPath(iconPath);
    dest.setBuildTargets(buildTargetList);

    return in;
}
