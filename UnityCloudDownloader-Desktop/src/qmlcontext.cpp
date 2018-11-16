#include "qmlcontext.h"

#include <QUrl>

QmlContext::QmlContext(QObject *parent)
    : QObject(parent)
{}

QString QmlContext::urlToPath(const QUrl &url) const
{
    return url.path();
}
