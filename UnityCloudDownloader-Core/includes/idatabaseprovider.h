#ifndef UCD_IDATABASEPROVIDER_H
#define UCD_IDATABASEPROVIDER_H

#pragma once

#include "unityclouddownloader-core_global.h"

class QSqlDatabase;

namespace ucd
{

/**
 * @brief The IDatabaseProvider interface.
 *
 * Provider interface for SQL connections.
 * The default SQL connection is limited to the UI thread.
 * Create a new connection on other threads by passing a connection name.
 */
class UCD_SHARED_EXPORT IDatabaseProvider
{
public:
    IDatabaseProvider() = default;
    virtual ~IDatabaseProvider() = default;
    IDatabaseProvider(const IDatabaseProvider&) = delete;
    IDatabaseProvider& operator=(const IDatabaseProvider&) = delete;

    /**
     * @brief Check if the database contains a least one profile.
     * @return true if there is a least one profile.
     * @note Only call from the UI thread.
     */
    virtual bool hasProfiles() const = 0;

    /**
     * @brief The default SQL connection, call from the UI thread only.
     * @return the default SQL connection.
     */
    virtual QSqlDatabase sqlDatabase() = 0;
    /**
     * @brief Create a new SQL connection that can be used in the calling thread only.
     * @param newConnectionName The name of the SQL connection.
     * @return A new SQL connection.
     */
    virtual QSqlDatabase sqlDatabase(const QString &newConnectionName) = 0;
};

}

#endif // UCD_IDATABASEPROVIDER_H
