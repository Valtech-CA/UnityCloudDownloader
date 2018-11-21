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
    ISynchronizer(const ISynchronizer&) = delete;
    virtual ~ISynchronizer() = default;

    ISynchronizer& operator=(const ISynchronizer&) = delete;

    virtual void manualDownload(const Build &build) = 0;

};

}

#endif // UCD_ISYNCHRONIZER_H
