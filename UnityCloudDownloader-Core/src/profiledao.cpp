#include "profiledao.h"

#include <QSqlQuery>

namespace ucd
{

ProfileDao::ProfileDao(QSqlDatabase &database)
    : m_db(database)
{

}

void ProfileDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS Profiles (profileId TEXT PRIMARY KEY, name TEXT, rootPath TEXT, apiKey TEXT)");
}

}
