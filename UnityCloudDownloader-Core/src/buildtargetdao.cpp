#include "buildtargetdao.h"

#include "buildtarget.h"

#include <QVariant>
#include <QSqlQuery>

namespace ucd
{

BuildTargetDao::BuildTargetDao(const QSqlDatabase &database)
    : m_db(database)
{}

BuildTargetDao::BuildTargetDao(QSqlDatabase &&database)
    : m_db(std::move(database))
{}

BuildTargetDao::~BuildTargetDao()
{}

void BuildTargetDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS BuildTargets (buildTargetId TEXT PRIMARY KEY, projectId TEXT, cloudId TEXT, name TEXT, platform TEXT)");
}

void BuildTargetDao::addBuildTarget(const BuildTarget &buildTarget)
{
    QSqlQuery query(m_db);
    query.prepare("INSET INTO BuildTargets (buildTargetId, projectId, cloudId, name, platform) "
                  "VALUES (:buildTargetId, :projectId, :cloudId, :name, :platform)");
    query.bindValue(":buildTargetId", buildTarget.id().toString());
    query.bindValue(":projectId", buildTarget.projectId().toString());
    query.bindValue(":cloudId", buildTarget.cloudId());
    query.bindValue(":name", buildTarget.name());
    query.bindValue(":platform", buildTarget.platform());
    query.exec();
}

void BuildTargetDao::updateBuildTarget(const BuildTarget &buildTarget)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE BuildTargets SET name = :name, platform = :platform "
                  "WHERE buildTargetId = :buildTargetId");
    query.bindValue(":name", buildTarget.name());
    query.bindValue(":platform", buildTarget.platform());
    query.bindValue(":buildTargetId", buildTarget.id());
    query.exec();
}

void BuildTargetDao::removeBuildTarget(const QUuid &buildTargetId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM BuildTargets WHERE buildTargetId = :buildTargetId");
    query.bindValue(":buildTargetId", buildTargetId.toString());
    query.exec();

    // TODO: delete builds
}

QVector<BuildTarget> BuildTargetDao::buildTargets(const QUuid &projectId, bool includeBuilds)
{
    QVector<BuildTarget> buildTargets;
    QSqlQuery query(m_db);
    if (projectId.isNull())
    {
        query.exec("SELECT * FROM BuildTargets");
    }
    else
    {
        query.prepare("SELECT * FROM BuildTargets WHERE projectId = :projectId");
        query.bindValue(":projectId", projectId.toString());
        query.exec();
    }
    while (query.next())
    {
        BuildTarget buildTarget;
        buildTarget.setId(query.value("buildTargetId").toString());
        buildTarget.setProjectId(query.value("projectId").toString());
        buildTarget.setCloudId(query.value("cloudId").toString());
        buildTarget.setName(query.value("name").toString());
        buildTarget.setPlatform(query.value("platform").toString());
        if (includeBuilds)
        {
            // TODO: add builds
        }
        buildTargets.append(std::move(buildTarget));
    }

    return buildTargets;
}

BuildTarget BuildTargetDao::buildTarget(const QUuid &buildTargetId, bool includeBuilds)
{
    BuildTarget buildTarget;

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM BuildTargets WHERE buildTargetId = :buildTargetId");
    query.bindValue(":buildTargetId", buildTargetId.toString());
    query.exec();
    if (query.next())
    {
        buildTarget.setId(query.value("buildTargetId").toString());
        buildTarget.setProjectId(query.value("projectId").toString());
        buildTarget.setCloudId(query.value("cloudId").toString());
        buildTarget.setName(query.value("name").toString());
        buildTarget.setPlatform(query.value("platform").toString());
        if (includeBuilds)
        {
            // TODO: inlcude builds
        }
    }
    else
    {
        // reset id to null to mark as invalid
        buildTarget.setId(QUuid());
    }

    return buildTarget;
}

void BuildTargetDao::removeBuildTargets(const QUuid &projectId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT buildTargetId FROM BuildTargets WHERE projectId = :projectId");
    query.bindValue(":projectId", projectId.toString());
    query.exec();
    while (query.next())
    {
        removeBuildTarget(query.value(0).toString());
    }
}

}
