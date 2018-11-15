#ifndef UCD_UNITYAPICLIENT_H
#define UCD_UNITYAPICLIENT_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QObject>

class QNetworkAccessManager;

namespace ucd
{

class UCD_SHARED_EXPORT UnityApiClient : public QObject
{
    Q_OBJECT
public:
    UnityApiClient(QObject *parent = nullptr);
    virtual ~UnityApiClient() override;

    Q_INVOKABLE void testKey(const QString &apiKey);

signals:
    void keyTested(bool isValid, QString apiKey);

private slots:
    void keyTestFinished();

private:
    QNetworkAccessManager *m_networkManager;
};

}

#endif // UCD_UNITYAPICLIENT_H
