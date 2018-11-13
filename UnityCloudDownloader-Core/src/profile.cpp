#include "profile.h"

#include <QDataStream>

namespace ucd
{

Profile::Profile()
    : m_uuid(QUuid::createUuid())
{}

Profile::Profile(const Profile &other)
    : m_uuid(other.m_uuid)
    , m_apiKey(other.m_apiKey)
    , m_rootPath(other.m_rootPath)
    , m_projects(other.m_projects)
{}

Profile::Profile(Profile &&other)
    : m_uuid(std::move(other.m_uuid))
    , m_apiKey(std::move(other.m_apiKey))
    , m_rootPath(std::move(other.m_rootPath))
    , m_projects(std::move(other.m_projects))
{}

Profile::~Profile()
{}

Profile &Profile::operator=(const Profile &other)
{
    new (this) Profile(other);
    return *this;
}

Profile &Profile::operator=(Profile &&other)
{
    new (this) Profile(std::move(other));
    return *this;
}

void Profile::setUuid(const QUuid &uuid)
{
    m_uuid = uuid;
}

void Profile::setName(const QString &name)
{
    m_name = name;
}

void Profile::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void Profile::setRootPath(const QString &rootPath)
{
    m_rootPath = rootPath;
}

void Profile::setProjects(const ProjectList &projects)
{
    m_projects = projects;
}

} // namespace ucd

QDataStream &operator<<(QDataStream &out, const ucd::ProjectList &value)
{
    out << value.size();
    for (const auto &item : value)
    {
        out << item;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::ProjectList &dest)
{
    int length;
    in >> length;
    for (int i = 0; i < length; ++i)
    {
        ucd::Project project;
        in >> project;
        dest.append(std::move(project));
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const ucd::Profile &value)
{
    out << value.uuid() << value.name() << value.apiKey() << value.rootPath() << value.projects();

    return out;
}

QDataStream &operator>>(QDataStream &in, ucd::Profile &dest)
{
    QUuid uuid;
    in >> uuid;
    QString name;
    in >> name;
    QString apiKey;
    in >> apiKey;
    QString rootPath;
    in >> rootPath;
    ucd::ProjectList projects;
    in >> projects;

    dest.setUuid(uuid);
    dest.setName(name);
    dest.setApiKey(apiKey);
    dest.setRootPath(rootPath);
    dest.setProjects(projects);

    return in;
}
