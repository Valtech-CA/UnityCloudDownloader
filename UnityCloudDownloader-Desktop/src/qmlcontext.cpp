#include "qmlcontext.h"

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
