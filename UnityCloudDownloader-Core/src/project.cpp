#include "project.h"

#include <QDataStream>

namespace ucd
{

Project::Project()
{}

Project::Project(const Project &other)
    : m_id(other.m_id)
    , m_profileId(other.m_profileId)
    , m_name(other.m_name)
    , m_orgId(other.m_orgId)
    , m_iconPath(other.m_iconPath)
    , m_buildTargets(other.m_buildTargets)
{}

Project::Project(Project &&other)
    : m_id(std::move(other.m_id))
    , m_profileId(std::move(other.m_profileId))
    , m_name(std::move(other.m_name))
    , m_orgId(std::move(other.m_orgId))
    , m_iconPath(std::move(other.m_iconPath))
    , m_buildTargets(std::move(other.m_buildTargets))
{}

Project::~Project()
{}

Project &Project::operator=(const Project &other)
{
    new (this) Project(other);
    return *this;
}

Project &Project::operator=(Project &&other)
{
    new (this) Project(std::move(other));
    return *this;
}

void Project::setName(const QString &name)
{
    m_name = name;
}

void Project::setId(const QString &id)
{
    m_id = id;
}

void Project::setProfileId(const QString &profileId)
{
    m_profileId = profileId;
}

void Project::setOrganisationId(const QString &id)
{
    m_id = id;
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
    out << value.name() << value.id() << value.organisationId() << value.iconPath() << value.buildTargets();
    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::Project &dest)
{
    QString name;
    in >> name;
    QString id;
    in >> id;
    QString orgId;
    in >> orgId;
    QString iconPath;
    in >> iconPath;
    ucd::BuildTargetList buildTargetList;
    in >> buildTargetList;

    dest.setName(name);
    dest.setId(id);
    dest.setOrganisationId(orgId);
    dest.setIconPath(iconPath);
    dest.setBuildTargets(buildTargetList);

    return in;
}
