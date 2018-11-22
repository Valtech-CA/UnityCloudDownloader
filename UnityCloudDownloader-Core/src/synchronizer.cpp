#include "synchronizer.h"

#include "build.h"

#include <QtConcurrent>

namespace ucd
{

Synchronizer::Synchronizer(QObject *parent)
    : AbstractSynchronizer(parent)
{

}

void Synchronizer::manualDownload(const Build &build)
{

}

} // namespace ucd
