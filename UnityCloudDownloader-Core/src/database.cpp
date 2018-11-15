#include "database.h"

#include "profiledao.h"
#include "projectdao.h"
#include "buildtargetdao.h"

#include <QSqlDatabase>
#include <QUuid>

namespace ucd
{

class DatabasePrivate
{
public:
    QString connectionName;
};

Database::Database(const QString &filePath, QObject *parent)
    : QObject(parent)
    , p(std::make_unique<DatabasePrivate>())
{
    p->connectionName = QUuid::createUuid().toString();
    auto database = QSqlDatabase::addDatabase("QSQLITE", p->connectionName);
    database.setDatabaseName(filePath);
    if (!database.open())
    {
        throw std::runtime_error("Cannot open database");
    }
}

Database::~Database()
{
    auto database = QSqlDatabase::database(p->connectionName);
    database.close();
}

void Database::init()
{
    auto database = QSqlDatabase::database(p->connectionName);
    ProfileDao(database).init();
    ProjectDao(database).init();
    BuildTargetDao(database).init();
}

QSqlDatabase Database::sqlDatabase()
{
    return QSqlDatabase::database(p->connectionName);
}

}
