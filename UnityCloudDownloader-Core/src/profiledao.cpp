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
    query.exec("CREATE TABLE IF NO EXISTS Profiles (uuid TEXT PRIMARY KEY, name TEXT, rootPath TEXT, apiKey TEXT)");
}

}
