#ifndef UCD_PROFILEDAO_H
#define UCD_PROFILEDAO_H

#pragma once

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class Profile;

class ProfileDao
{
public:
    ProfileDao(const QSqlDatabase &database);
    ~ProfileDao() = default;

    void init();

    void addProfile(const Profile &profile);
    void updateProfile(const Profile &profile);
    void removeProfile(const QUuid &profileId);
    QVector<Profile> profiles(bool includeProjects = false);
    Profile profile(const QUuid &profileId);

    QString getApiKey(const QUuid &profileId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_PROFILEDAO_H
