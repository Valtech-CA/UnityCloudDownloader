#include "database.h"

#include "profiledao.h"
#include "projectdao.h"
#include "buildtargetdao.h"
#include "builddao.h"
#include "downloadsdao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>
#include <QDir>

namespace ucd
{

class DatabasePrivate
{
public:
    QString storagePath;
    QString connectionName;
};

Database::Database(const QString &storagePath, QObject *parent)
    : QObject(parent)
    , p(std::make_unique<DatabasePrivate>())
{
    auto filePath = QDir(storagePath).filePath("data.sqlite");
    p->storagePath = filePath;
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
    BuildDao(database).init();
    DownloadsDao(database).init();
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

QSqlDatabase Database::sqlDatabase(const QString &newConnectionName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", newConnectionName);
    db.setDatabaseName(p->storagePath);
    return db;
}

}
