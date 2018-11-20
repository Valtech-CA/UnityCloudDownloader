#ifndef UCD_PROJECTDAO_H
#define UCD_PROJECTDAO_H

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class Project;

class ProjectDao
{
public:
    ProjectDao(const QSqlDatabase &database);
    ~ProjectDao() = default;

    void init();

    void addProject(const Project &project);
    void updateProject(const Project &project);
    void removeProject(const QUuid &projectId);
    QVector<Project> projects(const QUuid &profileId, bool includeBuildTargets = false);
    Project project(const QUuid &projectId, bool includeBuildTargets = false);

    void removeProjects(const QUuid &profileId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_PROJECTDAO_H
