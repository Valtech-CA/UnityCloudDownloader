#ifndef UCD_IDATABASEPROVIDER_H
#define UCD_IDATABASEPROVIDER_H

#pragma once

#include "unityclouddownloader-core_global.h"

class QSqlDatabase;

namespace ucd
{

class UCD_SHARED_EXPORT IDatabaseProvider
{
public:
    virtual ~IDatabaseProvider() = default;

    virtual bool hasProfiles() const = 0;

    virtual QSqlDatabase sqlDatabase() = 0;
    virtual QSqlDatabase sqlDatabase(const QString &newConnectionName) = 0;
};

}

#endif // UCD_IDATABASEPROVIDER_H
