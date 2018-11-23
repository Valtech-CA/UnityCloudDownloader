#include "qmlcontext.h"

#include "servicelocator.h"
#include "abstractsynchronizer.h"
#include "build.h"

#include <QUrl>
#include <QLocale>

QmlContext::QmlContext(QObject *parent)
    : QObject(parent)
{}

QString QmlContext::urlToPath(const QUrl &url) const
{
    return url.path();
}

QString QmlContext::formattedDataSize(qint64 bytes) const
{
    return QLocale().formattedDataSize(bytes);
}

void QmlContext::downloadManually(ucd::BuildRef build) const
{
    ucd::ServiceLocator::synchronizer()->manualDownload(build);
}
