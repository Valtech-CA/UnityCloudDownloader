#include "buildtargetdao.h"

#include <QSqlQuery>

namespace ucd
{

BuildTargetDao::BuildTargetDao(QSqlDatabase &database)
    : m_db(database)
{

}

void BuildTargetDao::init()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS BuildTargets (profileId TEXT, projectId TEXT, buildTargetId TEXT, name TEXT, platform TEXT, PRIMARY KEY (profileId, projectId, id))");
}

}
