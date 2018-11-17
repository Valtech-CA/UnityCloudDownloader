#include "projectdao.h"

#include "project.h"

#include <QSqlQuery>
#include <QVariant>

namespace ucd
{

ProjectDao::ProjectDao(QSqlDatabase &&database)
    : m_db(std::move(database))
{}

ProjectDao::ProjectDao(const QSqlDatabase &database)
    : m_db(database)
{}

ProjectDao::~ProjectDao()
{}

void ProjectDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS Projects (projectId TEXT PRIMARY KEY, profileId TEXT, cloudId TEXT, name TEXT, orgId TEXT, iconPath TEXT)");
}

void ProjectDao::addProject(const Project &project)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Projects (projectId, profileId, cloudId, name, orgId, iconPath) "
                  "VALUES (:projectId, :profileId, :cloudId, :name, :orgId, :iconPaht)");
    query.bindValue(":projectId", project.id().toString());
    query.bindValue(":profileId", project.profileId().toString());
    query.bindValue(":cloudId", project.cloudId());
    query.bindValue(":name", project.name());
    query.bindValue(":orgId", project.organisationId());
    query.bindValue(":iconPath", project.iconPath());
    query.exec();
}

void ProjectDao::updateProject(const Project &project)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Projecta SET name = :name, iconPath = :iconPath "
                  "WHERE projectId = :projectId");
    query.bindValue(":name", project.name());
    query.bindValue(":iconPath", project.iconPath());
    query.bindValue(":projectId", project.id());
    query.exec();
}

void ProjectDao::removeProject(const QUuid &projectId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Projects WHERE projectId = :projectId");
    query.bindValue(":projectId", projectId);
    query.exec();
    // TODO: delete buildTargets
}

QVector<Project> ProjectDao::projects(const QUuid &profileId, bool includeBuildTargets)
{
    QVector<Project> projects;
    QSqlQuery query(m_db);
    if (profileId.isNull())
    {
        query.exec("SELECT * FROM Projects");
    }
    else
    {
        query.prepare("SELECT * FROM Projects WHERE profileId = :profileId");
        query.bindValue(":profileId", profileId.toString());
        query.exec();
    }
    while (query.next())
    {
        Project project;
        project.setId(QUuid::fromString(query.value("projectId").toString()));
        project.setProfileId(QUuid::fromString(query.value("profileId").toString()));
        project.setCloudId(query.value("cloudId").toString());
        project.setName(query.value("name").toString());
        project.setOrganisationId(query.value("orgId").toString());
        project.setIconPath(query.value("iconPath").toString());
        if (includeBuildTargets)
        {
            // TODO: get the build targets
        }
        projects.append(std::move(project));
    }

    return projects;
}

Project ProjectDao::project(const QUuid &projectId, bool includeBuildTargets)
{
    Project project;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Projects WHERE projectId = :projectId");
    query.bindValue(":projectId", projectId.toString());
    query.exec();
    if (query.next())
    {
        project.setId(projectId);
        project.setProfileId(QUuid::fromString(query.value("profileId").toString()));
        project.setCloudId(query.value("cloudId").toString());
        project.setName(query.value("name").toString());
        project.setOrganisationId(query.value("orgId").toString());
        project.setIconPath(query.value("iconPath").toString());
        if (includeBuildTargets)
        {
            // TODO: get build targets
        }
    }
    else
    {
        // set project id to null to mark it as invalid
        project.setId(QUuid());
    }

    return project;
}

void ProjectDao::removeProjects(const QUuid &profileId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT projectId FROM Projects WHERE profileId = :profileId");
    query.bindValue(":profileId", profileId.toString());
    query.exec();
    while (query.next())
    {
        auto projectId = QUuid::fromString(query.value("projectId").toString());
        removeProject(projectId);
    }
}

}
