#include "downloadsdao.h"

#include <QVariant>
#include <QSqlError>

namespace ucd
{

DownloadsDao::DownloadsDao(const QSqlDatabase &database)
    : m_db(database)
{}

void DownloadsDao::init()
{
    QSqlQuery query(m_db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS Downloads ("
                    "buildTargetId TEXT, "
                    "buildNumber INT, "
                    "status INT, "
                    "PRIMARY KEY(buildTargetId, buildNumber))"))
    {
        auto error = query.lastError().text().toUtf8();
        qFatal("%s", error.data());
        throw std::runtime_error(error);
    }
}

QVector<BuildRef> DownloadsDao::downloadedBuilds(QUuid buildTargetId)
{
    QVector<BuildRef> builds;
    QSqlQuery query(m_db);
    if (buildTargetId.isNull())
    {
        query.prepare("SELECT buildTargetId, buildNumber "
                      "FROM Downloads "
                      "WHERE status = :status");
        query.bindValue(":status", Status::Downloaded);
    }
    else
    {
        query.prepare("SELECT buildTargetId, buildNumber "
                      "FROM Downloads "
                      "WHERE buildTargetId = :buildTargetId "
                      "AND status = :status");
        query.bindValue(":buildTargetId", buildTargetId.toString());
        query.bindValue(":status", Status::Downloaded);
    }
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical("%s", error.data());
        throw std::runtime_error(error);
    }

    while (query.next())
    {
        QUuid targetId = buildTargetId.isNull() ? query.value("buildTargetId").toUuid() : buildTargetId;
        int buildNumber = query.value("buildNumber").toInt();
        builds.append(BuildRef(targetId, buildNumber));
    }

    return builds;
}

void DownloadsDao::addDownload(BuildRef buildRef)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Downloads SET "
                  "status = :status "
                  "WHERE buildTargetId = :buildTargetId "
                  "AND buildNumber = :buildNumber");
    query.bindValue(":status", Status::Downloaded);
    query.bindValue(":buildTargetId", buildRef.buildTargetId().toString());
    query.bindValue(":buildNumber", buildRef.buildNumber());
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical("%s", error.data());
        throw std::runtime_error(error);
    }

    if (query.numRowsAffected() > 0)
        return;

    query.prepare("INSERT INTO Downloads ("
                  "buildTargetId, "
                  "buildNumber, "
                  "status) "
                  "VALUES ("
                  ":buildTargetId, "
                  ":buildNumber, "
                  ":status)");
    query.bindValue(":buildTargetId", buildRef.buildTargetId().toString());
    query.bindValue(":buildNumber", buildRef.buildNumber());
    query.bindValue(":status", Status::Downloaded);
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical("%s", error.data());
        throw std::runtime_error(error);
    }
}

void DownloadsDao::removeDownload(BuildRef buildRef)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Downloads "
                  "WHERE buildTargetId = :buildTargetId "
                  "AND buildNumber = :buildNumber");
    query.bindValue(":buildTargetId", buildRef.buildTargetId().toString());
    query.bindValue(":buildNumber", buildRef.buildNumber());
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical("%s", error.data());
        throw std::runtime_error(error);
    }
}

} //  namespace ucd
