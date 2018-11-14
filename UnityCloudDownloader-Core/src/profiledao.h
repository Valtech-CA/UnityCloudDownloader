#ifndef UCD_PROFILEDAO_H
#define UCD_PROFILEDAO_H

#include <QVector>

class QSqlDatabase;
class QUuid;

namespace ucd
{

class Profile;

class ProfileDao
{
public:
    ProfileDao(QSqlDatabase &database);

    void init();

    void addProfile(const Profile &profile);
    void updateProfile(const Profile &profile);
    void removeProfile(const QUuid &profileId);
    QVector<Profile> profiles(bool includeProjects = false);

private:
    QSqlDatabase &m_db;
};

}

#endif // UCD_PROFILEDAO_H
