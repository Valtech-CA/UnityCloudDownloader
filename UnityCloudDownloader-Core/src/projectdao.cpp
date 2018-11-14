#include "projectdao.h"

#include <QSqlQuery>

namespace ucd
{

ProjectDao::ProjectDao(QSqlDatabase &database)
    : m_db(database)
{
}

void ProjectDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS Projects (profileId TEXT, id TEXT, name TEXT, orgId TEXT, iconPath TEXT, PRIMARY KEY (profileId, id))");
}

}
