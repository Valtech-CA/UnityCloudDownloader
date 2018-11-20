#include "builddao.h"

#include "build.h"

#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

namespace ucd
{

BuildDao::BuildDao(const QSqlDatabase &database)
    : m_db(database)
{

}

void BuildDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS Builds ("
               "buildNumber INT, "
               "buildTargetId TEXT, "
               "status TINYINT, "
               "name TEXT, "
               "iconPath TEXT, "
               "artifactName TEXT, "
               "artifactSize BIGINT, "
               "artifactPath TEXT, "
               "manualDownload BOOLEAN,"
               "PRIMARY KEY(buildNumber, buildTargtId))");
}

void BuildDao::addBuild(const Build &build)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Builds ("
                  "buildNumber, buildTargetId, status, name, "
                  "iconPath, artifactName, artifactSize, "
                  "artifactPath, manualDownload)"
                  "VALUES (:buildNumber, :buildTargetId, :status, :name, "
                  ":iconPath, :artifactName, :artifactSize, "
                  ":artifactPath, :manualDownload)");
    query.bindValue(":buildNumber", build.id());
    query.bindValue(":buildTargetId", build.buildTargetId());
    query.bindValue(":status", build.status());
    query.bindValue(":name", build.name());
    query.bindValue(":iconPath", build.iconPath());
    query.bindValue(":artifactName", build.artifactName());
    query.bindValue(":artifactSize", build.artifactSize());
    query.bindValue(":artifactPath", build.artifactPath());
    query.bindValue(":manualDownload", build.manualDownload());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

void BuildDao::updateBuild(const Build &build)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Builds SET "
                  "status = :status, "
                  "iconPath = :iconPath, "
                  "artifactName = :artifactName, "
                  "artifactSize = :artifactSize, "
                  "artifactPath = :artifactPath, "
                  "manualDownload = :manualDownlaod "
                  "WHERE buildNumber = :buildNumber "
                  "AND buildTargetId = :buildTargetId");
    query.bindValue(":status", build.status());
    query.bindValue(":iconPath", build.iconPath());
    query.bindValue(":artifactName", build.artifactName());
    query.bindValue(":artifactSize", build.artifactSize());
    query.bindValue(":artifactPath", build.artifactPath());
    query.bindValue(":manualDownload", build.manualDownload());
    query.bindValue(":buildNumber", build.id());
    query.bindValue(":buildtargetId", build.buildTargetId().toString());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

void BuildDao::removeBuild(const Build &build)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Builds "
                  "WHERE buildNumber = :buildNumber "
                  "AND buildTargetId = :buildTargetId");
    query.bindValue(":buildNumber", build.id());
    query.bindValue(":buildTargetId", build.buildTargetId().toString());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

QVector<Build> BuildDao::builds(const QUuid &buildTargetId)
{
    QVector<Build> builds;
    QSqlQuery query(m_db);
    if (buildTargetId.isNull())
    {
        query.exec("SELECT * FROM Builds");
    }
    else
    {
        query.prepare("SELECT * FROM Builds WHERE buildTargetId = :buildTargetId");
        query.bindValue(":buildTargetId", buildTargetId.toString());
        query.exec();
    }
    while (query.next())
    {
        Build build;
        build.setId(query.value("buildNumber").toInt());
        build.setBuildTargetId(query.value("buildTargetId").toString());
        build.setStatus(query.value("status").toInt());
        build.setName(query.value("name").toString());
        build.setIconPath(query.value("iconPath").toString());
        build.setArtifactName(query.value("artifactName").toString());
        build.setArtifactSize(query.value("artifactSize").toULongLong());
        build.setArtifactPath(query.value("artifactPath").toString());
        build.setManualDownload(query.value("manualDownload").toBool());
        builds.append(std::move(build));
    }

    return builds;
}

void BuildDao::removeBuilds(const QUuid &buildTargetId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Builds WHERE buildTargetId = :buildTargetId");
    query.bindValue(":buildTargetId", buildTargetId.toString());
    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toUtf8());
    }
}

} // namespace ucd
