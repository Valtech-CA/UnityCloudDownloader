#include "unityapiclient.h"

#include "project.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace ucd
{

UnityApiClient::UnityApiClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(nullptr)
{
    m_networkManager = new QNetworkAccessManager(this);
}

UnityApiClient::~UnityApiClient()
{}

void UnityApiClient::testKey(const QString &apiKey)
{
    QNetworkRequest request(QUrl{"https://build-api.cloud.unity3d.com/api/v1/users/me"});
    request.setRawHeader("Authorization", QString("Basic %1").arg(apiKey).toUtf8());

    auto *reply = m_networkManager->get(request);
    reply->setProperty("unityApiKey", apiKey);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::keyTestFinished);
}

void UnityApiClient::fetchProjects(const QString &apiKey)
{
    QNetworkRequest request(QUrl{"https://build-api.cloud.unity3d.com/api/v1/projects"});
    request.setRawHeader("Authorization", QString("Basic %1").arg(apiKey).toUtf8());

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::projectsReceived);
}

void UnityApiClient::keyTestFinished()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    bool valid = reply->error() == 0;
    QString apiKey = reply->property("unityApiKey").toString();
    emit keyTested(valid, apiKey);
    reply->deleteLater();
}

void UnityApiClient::projectsReceived()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    QVector<Project> projects;

    if (reply->error() == 0)
    {

    }

    emit projectsFetched(projects);
    reply->deleteLater();
}

}
