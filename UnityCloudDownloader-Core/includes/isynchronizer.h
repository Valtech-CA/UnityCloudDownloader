#ifndef UCD_ISYNCHRONIZER_H
#define UCD_ISYNCHRONIZER_H

#pragma once

#include "unityclouddownloader-core_global.h"

namespace ucd
{

class Build;

/**
 * @brief The ISynchronizer interface
 *
 * Inteface for syncrhonizing cloud builds to local drive.
 */
class UCD_SHARED_EXPORT ISynchronizer
{
public:
    ISynchronizer() = default;
    virtual ~ISynchronizer() = default;
    ISynchronizer(const ISynchronizer&) = delete;
    ISynchronizer& operator=(const ISynchronizer&) = delete;

    /**
     * @brief Query if the build is queued for download.
     * @param build the build that is queried.
     * @return true if the build is queued for download.
     */
    virtual bool isQueued(const Build &build) const = 0;
    /**
     * @brief Query if the build is downloaded.
     * @param build the build that is queried.
     * @return true if the build is donwloaded.
     */
    virtual bool isDownloaded(const Build &build) const = 0;
    /**
     * @brief Query if the build is currently downloading.
     * @param build the build that is queried.
     * @return true if the build is currently downloading.
     */
    virtual bool isDownloading(const Build &build) const = 0;
    /**
     * @brief Query the download progress of a build.
     * @param build the build that is queried.
     * @return a value in [0..1] representing the download progress.
     */
    virtual float downloadProgress(const Build &build) const = 0;
    /**
     * @brief Query the download speed of a build.
     * @param build the build that is queried.
     * @return a value >= 0 representing the aproximate download speed in bytes per second.
     */
    virtual qint64 downloadSpeed(const Build &build) const = 0;
    /**
     * @brief Request a manual download of a build.
     * @param build the build to download.
     */
    virtual void manualDownload(const Build &build) = 0;
    /**
     * @brief Refresh synchronization.
     *
     * Cause the syncrhonizer to get the latest list of builds from the Unity cloud,
     * compare it the syncrhonization configuration and download or delete builds as needed.
     *
     * @note This operation is asyncrhonous.
     */
    virtual void refresh() = 0;
};

}

#endif // UCD_ISYNCHRONIZER_H
