#ifndef UCD_PROFILEDAO_H
#define UCD_PROFILEDAO_H

#include <QVector>

class QSqlDatabase;

namespace ucd
{

class Profile;

class ProfileDao
{
public:
    ProfileDao(QSqlDatabase &database);

    void init();

    QVector<Profile> profiles(bool includeProjects = false);

private:
    QSqlDatabase &m_db;
};

}

#endif // UCD_PROFILEDAO_H
