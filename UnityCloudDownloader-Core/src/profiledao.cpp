#include "profiledao.h"

#include "profile.h"
#include "projectdao.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace ucd
{

ProfileDao::ProfileDao(const QSqlDatabase &database)
    : m_db(database)
{}

void ProfileDao::init()
{
    QSqlQuery query(m_db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS "
               "Profiles (profileId TEXT PRIMARY KEY, name TEXT, rootPath TEXT, apiKey TEXT)"))
    {
        auto error = query.lastError().text().toUtf8();
        qFatal(error);
        throw std::runtime_error(error);
    }
}

void ProfileDao::addProfile(const Profile &profile)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Profiles (profileId, name, rootPath, apiKey) "
                  "VALUES (:profileId, :name, :rootPath, :apiKey)");
    query.bindValue(":profileId", profile.uuid().toString());
    query.bindValue(":name", profile.name());
    query.bindValue(":rootPath", profile.rootPath());
    query.bindValue(":apiKey", profile.apiKey());
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical(error);
        throw std::runtime_error(error);
    }
}

void ProfileDao::updateProfile(const Profile &profile)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Profiles SET name = :name, rootPath = :rootPath, apiKey = :apiKey "
                  "WHERE profileId = :profileId");
    query.bindValue(":profileId", profile.uuid().toString());
    query.bindValue(":name", profile.name());
    query.bindValue(":rootPath", profile.rootPath());
    query.bindValue(":apiKey", profile.apiKey());
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical(error);
        throw std::runtime_error(error);
    }
}

void ProfileDao::removeProfile(const QUuid &profileId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Profiles WHERE profileId = :profileId");
    query.bindValue(":profileId", profileId.toString());
    if (!query.exec())
    {
        auto error = query.lastError().text().toUtf8();
        qCritical(error);
        throw std::runtime_error(error);
    }

    ProjectDao(m_db).removeProjects(profileId);
}

QVector<Profile> ProfileDao::profiles(bool includeProjects)
{
    QVector<Profile> profiles;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM Profiles");
    while (query.next())
    {
        Profile profile;
        profile.setUuid(QUuid::fromString(query.value("profileId").toString()));
        profile.setName(query.value("name").toString());
        profile.setRootPath(query.value("rootPath").toString());
        profile.setApiKey(query.value("apiKey").toString());
        if (includeProjects)
        {
            profile.setProjects(ProjectDao(m_db).projects(profile.uuid(), true));
        }
        profiles.append(std::move(profile));
    }

    return profiles;
}

QString ProfileDao::getApiKey(const QUuid &profileId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT apiKey FROM Profiles WHERE profileId = :profileId");
    query.bindValue(":profileId", profileId.toString());
    query.exec();

    if (query.next())
    {
        return query.value("apiKey").toString();
    }

    return "";
}

}
