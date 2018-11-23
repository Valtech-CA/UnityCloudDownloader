#ifndef UCD_DATABASE_H
#define UCD_DATABASE_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "idatabaseprovider.h"

#include <QObject>

class QSqlDatabase;

namespace ucd
{

class DatabasePrivate;

class Database : public QObject, public IDatabaseProvider
{
    Q_OBJECT

public:
    explicit Database(const QString &storagePath, QObject *parent = nullptr);
    ~Database() override;

    void init();

    bool hasProfiles() const override;

    QSqlDatabase sqlDatabase() override;
    QSqlDatabase sqlDatabase(const QString &newConnectionName) override;

private:
    std::unique_ptr<DatabasePrivate> p;
};

}

#endif // UCD_DATABASE_H
