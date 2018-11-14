#ifndef UCD_DATABASE_H
#define UCD_DATABASE_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QObject>


namespace ucd
{

class DatabasePrivate;

class UCD_SHARED_EXPORT Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(const QString &filePath, QObject *parent = nullptr);
    ~Database();

    void init();

private:
    std::unique_ptr<DatabasePrivate> p;
};

}

#endif // UCD_DATABASE_H
