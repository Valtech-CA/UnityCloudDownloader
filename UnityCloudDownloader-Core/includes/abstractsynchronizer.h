#ifndef UCD_ABSTRACTSYNCHRONIZER_H
#define UCD_ABSTRACTSYNCHRONIZER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "isynchronizer.h"
#include "build.h"

#include <QObject>

namespace ucd
{

class Database;

/**
 * @brief The AbstractSynchronizer class
 *
 * Provides base abstract implementation of the ISyncrhonizer interface with signals.
 */
class UCD_SHARED_EXPORT AbstractSynchronizer : public QObject, public ISynchronizer
{
    Q_OBJECT
public:
    AbstractSynchronizer(QObject *parent = nullptr);
    AbstractSynchronizer(const AbstractSynchronizer&) = delete;
    virtual ~AbstractSynchronizer() override = default;

    AbstractSynchronizer& operator=(const AbstractSynchronizer&) = delete;

signals:
    /**
     * @brief Signal emitted when a build is queued for download.
     * @param build the build queued for download.
     */
    void downloadQueued(ucd::Build build);
    /**
     * @brief Signal emitted when a build download is started.
     * @param build the build that started downloading.
     */
    void downloadStarted(ucd::Build build);
    /**
     * @brief Signal emitted when a build download has updated (progressed).
     * @param build the build that is being downloaded.
     */
    void downloadUpdated(ucd::Build build);
    /**
     * @brief Signal emitted when a build as completed downloaded.
     * @param build the build that completed download.
     */
    void downloadCompleted(ucd::Build build);
    /**
     * @brief Signal emitted when a build download has failed.
     * @param build the build that failed downloading.
     */
    void downloadFailed(ucd::Build build);
    /**
     * @brief The syncrhonizer has completed the asynchronous refresh cycle.
     */
    void synchronized();
};

}

#endif // UCD_ABSTRACTSYNCHRONIZER_H
