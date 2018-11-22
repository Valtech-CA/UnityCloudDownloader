#ifndef UCD_CORE_H
#define UCD_CORE_H

#include "unityclouddownloader-core_global.h"

class QObject;

namespace ucd
{

class UCD_SHARED_EXPORT Core
{
public:
    Core() = delete;
    Core(const Core&) = delete;
    ~Core() = delete;
    Core& operator=(const Core&) = delete;

    static void init();
    static void init(const QString &storagePath, QObject *parent);
};

}

#endif // UCD_CORE_H
