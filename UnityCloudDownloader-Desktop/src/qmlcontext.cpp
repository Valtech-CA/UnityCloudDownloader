#include "qmlcontext.h"

#include "servicelocator.h"
#include "abstractsynchronizer.h"
#include "build.h"

#include <QUrl>
#include <QLocale>
#include <QFileInfo>
#include <QDesktopServices>

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

void QmlContext::openBuildFolder(ucd::BuildRef build) const
{
    auto dirPath = ucd::Build(build).downloadFolderPath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void QmlContext::refreshSync() const
{
    ucd::ServiceLocator::synchronizer()->refresh();
}
