#include "unityapiclient.h"

#include "profile.h"
#include "profiledao.h"
#include "project.h"
#include "projectdao.h"
#include "buildtarget.h"
#include "build.h"
#include "servicelocator.h"

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

void UnityApiClient::fetchProjects(const Profile &profile)
{
    QNetworkRequest request(QUrl{API("/projects")});
    setAuthorization(request, profile.apiKey());

    auto *reply = m_networkManager->get(request);
    reply->setProperty("profile", QVariant::fromValue(profile));
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::projectsReceived);
}

void UnityApiClient::fetchBuildTargets(const QString &orgId, const QString &projectId)
{
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets").arg(orgId, projectId)});
    setAuthorization(request, m_apiKey);

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::buildTargetsReceived);
}

void UnityApiClient::fetchBuildTargets(const Project &project)
{
    auto apiKey = ProfileDao(ServiceLocator::database()).getApiKey(project.profileId());
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets").arg(project.organisationId(), project.cloudId())});
    setAuthorization(request, apiKey);

    auto *reply = m_networkManager->get(request);
    reply->setProperty("project", QVariant::fromValue(project));
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::buildTargetsReceived);
}

void UnityApiClient::fetchBuilds(const QString &orgId, const QString &projectId, const QString &buildTargetId)
{
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets/%3/builds").arg(orgId, projectId, buildTargetId)});
    setAuthorization(request, m_apiKey);

    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UnityApiClient::buildsReceived);
}

void UnityApiClient::fetchBuilds(const BuildTarget &buildTarget)
{
    auto project = ProjectDao(ServiceLocator::database()).project(buildTarget.projectId());
    auto apiKey = ProfileDao(ServiceLocator::database()).getApiKey(project.profileId());
    auto orgId = project.organisationId();
    auto projectId = project.cloudId();
    auto buildTargetId = buildTarget.cloudId();
    QNetworkRequest request(QUrl{API("/orgs/%1/projects/%2/buildtargets/%3/builds").arg(orgId, projectId, buildTargetId)});
    setAuthorization(request, apiKey);

    auto *reply = m_networkManager->get(request);
    reply->setProperty("buildTarget", QVariant::fromValue(buildTarget));
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

    auto profileProperty = reply->property("profile");
    QUuid profileId;
    if (profileProperty.isValid())
        profileId = profileProperty.value<Profile>().uuid();

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
            project.setProfileId(profileId);
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

    auto projectProperty = reply->property("project");
    QUuid projectId;
    if (projectProperty.isValid())
        projectId = projectProperty.value<Project>().id();

    if (reply->error() == 0)
    {
        auto replyData = reply->readAll();
        auto jsonDocument = QJsonDocument::fromJson(replyData);
        auto jsonData = jsonDocument.array();
        for (QJsonValue value : jsonData)
        {
            BuildTarget buildTarget;
            buildTarget.setName(value["name"].toString());
            buildTarget.setProjectId(projectId);
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

    auto buildTargetProperty = reply->property("buildTarget");
    QUuid buildTargetId;
    if (buildTargetProperty.isValid())
        buildTargetId = buildTargetProperty.value<BuildTarget>().id();

    if (reply->error() == 0)
    {
        auto replyData = reply->readAll();
        auto jsonDocument = QJsonDocument::fromJson(replyData);
        auto jsonData = jsonDocument.array();
        for (QJsonValue value : jsonData)
        {
            Build build;
            build.setId(value["build"].toInt());
            build.setBuildTargetId(buildTargetId);
            build.setName(value["buildTargetName"].toString());
            build.setStatus(Build::statusFromString(value["buildStatus"].toString()));
            build.setCreateTime(value["created"].toVariant().toDateTime());
            auto links = value["links"];
            if (links.isObject())
            {
                auto icon = links["icon"];
                if (icon.isObject())
                {
                    build.setIconPath(icon["href"].toString());
                }
                auto artifacts = links["artifacts"].toArray();
                for (QJsonValue artifact : artifacts)
                {
                    if (artifact["key"].toString() != QStringLiteral("primary"))
                        continue;
                    auto files = artifact["files"].toArray();
                    if (!files.isEmpty())
                    {
                        QJsonValue file = files[0];
                        build.setArtifactName(file["filename"].toString());
                        build.setArtifactSize(file["size"].toVariant().toLongLong());
                        build.setArtifactPath(file["href"].toString());
                        break;
                    }
                    break;
                }
            }

            builds.append(std::move(build));
        }
    }

    emit buildsFetched(builds);
}

}
