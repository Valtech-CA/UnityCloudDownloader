#include "servicelocator.h"

#include "idatabaseprovider.h"

#include <QSqlDatabase>

namespace ucd
{

IDatabaseProvider* ServiceLocator::m_databaseProvider = nullptr;
AbstractSynchronizer* ServiceLocator::m_synchronizer = nullptr;

QSqlDatabase ServiceLocator::database()
{
    if (m_databaseProvider != nullptr)
        return m_databaseProvider->sqlDatabase();

    return {};
}

void ServiceLocator::setDatabaseProvier(IDatabaseProvider *databaseProvider)
{
    m_databaseProvider = databaseProvider;
}

void ServiceLocator::setSynchronizer(AbstractSynchronizer *synchronizer)
{
    m_synchronizer = synchronizer;
}

} // namespace ucd
