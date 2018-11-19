#ifndef UCD_UNITYAPICLIENT_H
#define UCD_UNITYAPICLIENT_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include "project.h"
#include "buildtarget.h"

#include <QObject>
#include <QVector>

class QNetworkAccessManager;

namespace ucd
{

class UCD_SHARED_EXPORT UnityApiClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
public:
    UnityApiClient(QObject *parent = nullptr);
    UnityApiClient(const QString &apiKey, QObject *parent = nullptr);
    virtual ~UnityApiClient() override;

    const QString& apiKey() const { return m_apiKey; }
    void setApiKey(const QString &apiKey);

    Q_INVOKABLE void testKey(const QString &apiKey);

    Q_INVOKABLE void fetchProjects();
    Q_INVOKABLE void fetchBuildTargets(const QString &orgId, const QString &projectId);

signals:
    void apiKeyChanged(QString apiKey);
    void keyTested(bool isValid, QString apiKey);
    void projectsFetched(QVector<Project> projects);
    void buildTargetsFetched(QVector<BuildTarget> buildTargets);

private slots:
    void keyTestFinished();
    void projectsReceived();
    void buildTargetsReceived();

private:
    QString m_apiKey;
    QNetworkAccessManager *m_networkManager;
};

}

#endif // UCD_UNITYAPICLIENT_H
