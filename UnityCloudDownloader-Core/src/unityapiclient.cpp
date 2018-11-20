#include "unityapiclient.h"

#include "project.h"
#include "buildtarget.h"
#include "build.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#define API(endPoint) QStringLiteral("https://build-api.cloud.unity3d.com/api/v1" endPoint)

namespace ucd
{

static void setAuthorization(QNetworkRequest &request, const QString &apiKey)
{
    request.setRawHeader("Authorization", QStringLiteral("Basic %1").arg(apiKey).toUtf8());
}

UnityApiClient::UnityApiClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(nullptr)
{
    m_networkManager = new QNetworkAccessManager(this);
}

UnityApiClient::UnityApiClient(QString apiKey, QObject *parent)
    : QObject(parent)
    , m_apiKey(std::move(apiKey))
    , m_networkManager(nullptr)
{
    m_networkManager = new QNetworkAccessManager(this);
}

void UnityApiClient::setApiKey(const QString &apiKey)
{
    if (apiKey == m_apiKey)
        return;

    m_apiKey = apiKey;
    emit apiKeyChanged(apiKey);
}

void UnityApiClient::testKey(const QString &apiKey)
{
    QNetworkRequest request(QUrl{API("/users/me")});
    setAuthorization(request, apiKey);

    auto *reply = m_networkManager->get(request);
    reply->setProperty("unityApiKey", apiKey);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::keyTestFinished);
}

void UnityApiClient::fetchProjects()
{
    QNetworkRequest request(QUrl{API("/projects")});
    setAuthorization(request, m_apiKey);

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::projectsReceived);
}

void UnityApiClient::fetchBuildTargets(const QString &orgId, const QString &projectId)
{
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets").arg(orgId, projectId)});
    setAuthorization(request, m_apiKey);

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::buildTargetsReceived);
}

void UnityApiClient::fetchBuilds(const QString &orgId, const QString &projectId, const QString &buildTargetId)
{
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets/%3/builds").arg(orgId, projectId, buildTargetId)});
    setAuthorization(request, m_apiKey);

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::buildsReceived);
}

void UnityApiClient::preconnect()
{
    QNetworkAccessManager manager;
    manager.connectToHostEncrypted(API(""));
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
    reply->deleteLater();
    QVector<Project> projects;

    if (reply->error() == 0)
    {
        auto replyData = reply->readAll();
        auto jsonDocument = QJsonDocument::fromJson(replyData);
        auto jsonProjects = jsonDocument.array();
        for (QJsonValue value : jsonProjects)
        {
            if (value["disabled"].toBool())
                continue;

            Project project;
            project.setName(value["name"].toString());
            project.setCloudId(value["projectid"].toString());
            project.setOrganisationId(value["orgid"].toString());
            project.setIconPath(value["cachedIcon"].toString());

            projects.append(std::move(project));
        }
    }

    emit projectsFetched(projects);
}

void UnityApiClient::buildTargetsReceived()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    QVector<BuildTarget> buildTargets;

    if (reply->error() == 0)
    {
        auto replyData = reply->readAll();
        auto jsonDocument = QJsonDocument::fromJson(replyData);
        auto jsonData = jsonDocument.array();
        for (QJsonValue value : jsonData)
        {
            BuildTarget buildTarget;
            buildTarget.setName(value["name"].toString());
            buildTarget.setCloudId(value["buildtargetid"].toString());
            buildTarget.setPlatform(value["platform"].toString());

            buildTargets.append(std::move(buildTarget));
        }
    }

    emit buildTargetsFetched(buildTargets);
}

void UnityApiClient::buildsReceived()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    QVector<Build> builds;

    if (reply->error() == 0)
    {
        auto replyData = reply->readAll();
        auto jsonDocument = QJsonDocument::fromJson(replyData);
        auto jsonData = jsonDocument.array();
        for (QJsonValue value : jsonData)
        {
            Build build;
            build.setId(value["build"].toInt());
            build.setName(value["buildTargetName"].toString());
            build.setStatus(Build::statusFromString(value["buildStatus"].toString()));
            auto links = value["links"];
            if (links.isObject())
            {
                auto icon = links["icon"];
                if (icon.isObject())
                {
                    build.setIconPath(icon["href"].toString());
                }
            }
            auto artifacts = value["artifacts"].toArray();
            for (QJsonValue artifact : artifacts)
            {
                if (artifact["key"].toString() != QStringLiteral("primary"))
                    continue;
                auto files = artifact["files"].toArray();
                if (!files.isEmpty())
                {
                    QJsonValue file = files[0];
                    build.setArtifactName(file["name"].toString());
                    build.setArtifactSize(file["size"].toVariant().toULongLong());
                    build.setArtifactPath(file["href"].toString());
                    break;
                }
                break;
            }
        }
    }

    emit buildsFetched();
}

}
