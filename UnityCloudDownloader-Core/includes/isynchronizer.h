#ifndef UCD_ISYNCHRONIZER_H
#define UCD_ISYNCHRONIZER_H

#pragma once

#include "unityclouddownloader-core_global.h"

namespace ucd
{

class Build;

class UCD_SHARED_EXPORT ISynchronizer
{
public:
    virtual ~ISynchronizer() = default;

    virtual bool isQueued(const Build &build) const = 0;
    virtual bool isDownloaded(const Build &build) const = 0;
    virtual bool isDownloading(const Build &build) const = 0;
    virtual float downloadProgress(const Build &build) const = 0;
    virtual qint64 downloadSpeed(const Build &build) const = 0;
    virtual void manualDownload(const Build &build) = 0;
    virtual void refresh() = 0;
};

}

#endif // UCD_ISYNCHRONIZER_H
