#ifndef QMLCONTEXT_H
#define QMLCONTEXT_H

#include <QObject>

class QmlContext : public QObject
{
    Q_OBJECT
public:
    explicit QmlContext(QObject *parent = nullptr);

    Q_INVOKABLE QString urlToPath(const QUrl &url) const;

    Q_INVOKABLE QString formattedDataSize(qint64 bytes) const;
};

#endif // QMLCONTEXT_H
