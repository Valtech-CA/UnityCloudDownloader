#ifndef UCD_PROFILEDAO_H
#define UCD_PROFILEDAO_H

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class Profile;

class ProfileDao
{
public:
    ProfileDao(QSqlDatabase &&database);
    ProfileDao(const QSqlDatabase &databse);
    ~ProfileDao();

    void init();

    void addProfile(const Profile &profile);
    void updateProfile(const Profile &profile);
    void removeProfile(const QUuid &profileId);
    QVector<Profile> profiles(bool includeProjects = false);

    QString getApiKey(const QUuid &profileId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_PROFILEDAO_H
