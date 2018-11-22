#ifndef UCD_SERVICELOCATOR_H
#define UCD_SERVICELOCATOR_H

#pragma once

#include "unityclouddownloader-core_global.h"

class QSqlDatabase;

namespace ucd
{

class IDatabaseProvider;
class AbstractSynchronizer;

class UCD_SHARED_EXPORT ServiceLocator
{
public:
    ServiceLocator() = delete;
    ServiceLocator(const ServiceLocator&) = delete;
    ~ServiceLocator() = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    static QSqlDatabase database();

    static IDatabaseProvider* databaseProvider() { return m_databaseProvider; }
    static AbstractSynchronizer* synchronizer() { return m_synchronizer; }

    static void setDatabaseProvier(IDatabaseProvider *databaseProvider);
    static void setSynchronizer(AbstractSynchronizer *synchronizer);

private:
    static IDatabaseProvider *m_databaseProvider;
    static AbstractSynchronizer *m_synchronizer;
};

}

#endif // UCD_SERVICELOCATOR_H
