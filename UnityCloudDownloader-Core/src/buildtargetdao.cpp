#include "buildtargetdao.h"

#include "buildtarget.h"

#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

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
    query.exec("CREATE TABLE IF NOT EXISTS BuildTargets ("
               "buildTargetId TEXT PRIMARY KEY, "
               "projectId TEXT, "
               "cloudId TEXT, "
               "name TEXT, "
               "platform TEXT, "
               "sync BOOLEAN, "
               "minBuilds INT, "
               "maxBuilds INT, "
               "maxDaysOld INT)");
}

void BuildTargetDao::addBuildTarget(const BuildTarget &buildTarget)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO BuildTargets ("
                  "buildTargetId, projectId, cloudId, name, platform, "
                  "sync, minBuilds, maxBuilds, maxDaysOld) "
                  "VALUES (:buildTargetId, :projectId, :cloudId, :name, :platform, "
                  ":sync, :minBuilds, :maxBuilds, :maxDaysOld)");
    query.bindValue(":buildTargetId", buildTarget.id().toString());
    query.bindValue(":projectId", buildTarget.projectId().toString());
    query.bindValue(":cloudId", buildTarget.cloudId());
    query.bindValue(":name", buildTarget.name());
    query.bindValue(":platform", buildTarget.platform());
    query.bindValue(":sync", buildTarget.sync());
    query.bindValue(":minBuilds", buildTarget.minBuilds());
    query.bindValue(":maxBuilds", buildTarget.maxBuilds());
    query.bindValue(":maxDaysOld", buildTarget.maxDaysOld());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

void BuildTargetDao::updateBuildTarget(const BuildTarget &buildTarget)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE BuildTargets SET name = :name, platform = :platform, "
                  "sync = :sync, minBuilds = :minBuilds, maxBuilds = :maxBuilds, maxDaysOld = :maxDaysOld "
                  "WHERE buildTargetId = :buildTargetId");
    query.bindValue(":name", buildTarget.name());
    query.bindValue(":platform", buildTarget.platform());
    query.bindValue(":buildTargetId", buildTarget.id());
    query.bindValue(":sync", buildTarget.sync());
    query.bindValue(":minBuilds", buildTarget.minBuilds());
    query.bindValue(":maxBuilds", buildTarget.maxBuilds());
    query.bindValue(":maxDaysOld", buildTarget.maxDaysOld());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

void BuildTargetDao::removeBuildTarget(const QUuid &buildTargetId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM BuildTargets WHERE buildTargetId = :buildTargetId");
    query.bindValue(":buildTargetId", buildTargetId.toString());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }

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
        buildTarget.setSync(query.value("sync").toBool());
        buildTarget.setMinBuilds(query.value("minBuilds").toInt());
        buildTarget.setMaxBuilds(query.value("maxBuilds").toInt());
        buildTarget.setMaxDaysOld(query.value("maxDaysOld").toInt());
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
        buildTarget.setSync(query.value("sync").toBool());
        buildTarget.setMinBuilds(query.value("minBuilds").toInt());
        buildTarget.setMaxBuilds(query.value("maxBuilds").toInt());
        buildTarget.setMaxDaysOld(query.value("maxDaysOld").toInt());
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
