#include "database.h"

#include "profiledao.h"
#include "projectdao.h"
#include "buildtargetdao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

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
    //BuildTargetDao(database).init();
}

bool Database::hasProfiles() const
{
    auto database = QSqlDatabase::database(p->connectionName);
    QSqlQuery query(database);
    query.exec("SELECT COUNT(*) WHERE EXISTS(SELECT 1 FROM Profiles)");
    if (query.next())
    {
        return query.value(0).toInt() != 0;
    }
    return false;
}

QSqlDatabase Database::sqlDatabase()
{
    return QSqlDatabase::database(p->connectionName);
}

}
