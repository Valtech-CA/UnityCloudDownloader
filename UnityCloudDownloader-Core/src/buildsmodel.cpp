#include "buildsmodel.h"

#include "build.h"
#include "builddao.h"
#include "buildtarget.h"
#include "buildtargetdao.h"
#include "project.h"
#include "projectdao.h"
#include "profiledao.h"
#include "database.h"
#include "unityapiclient.h"

#include <algorithm>

#include <QSqlDatabase>

namespace ucd
{

BuildsModel::BuildsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_db(nullptr)
{}

BuildsModel::~BuildsModel()
{}

void BuildsModel::setDatabase(Database *database)
{
    if (database == m_db)
        return;

    beginResetModel();
    m_db = database;
    if (m_db != nullptr && !m_buildTargetId.isNull())
    {
        m_builds = BuildDao(m_db->sqlDatabase()).builds(m_buildTargetId);
        fetchMore(QModelIndex());
    }
    else
    {
        m_builds.clear();
    }

    endResetModel();
    emit databaseChanged(database);
}

void BuildsModel::setBuildTargetId(const QUuid &buildTargetId)
{
    if (buildTargetId == m_buildTargetId)
        return;

    beginResetModel();
    m_buildTargetId = buildTargetId;
    if (m_db != nullptr && !m_buildTargetId.isNull())
    {
        m_builds = BuildDao(m_db->sqlDatabase()).builds(m_buildTargetId);
        fetchMore(QModelIndex());
    }
    else
    {
        m_builds.clear();
    }

    endResetModel();
    emit buildTargetIdChanged(buildTargetId);
}

bool BuildsModel::updateBuild(int row, const Build &build)
{
    if (row >= m_builds.size())
        return false;

    auto &currentBuild = m_builds[row];

    if (currentBuild.isLike(build))
        return true;

    currentBuild.takeFrom(build);

    BuildDao(m_db->sqlDatabase()).updateBuild(currentBuild);
    emit dataChanged(index(row), index(row));
    return true;
}

void BuildsModel::addBuild(const Build &build)
{
    beginInsertRows(QModelIndex(), m_builds.size(), m_builds.size());
    Build newBuild(build);
    newBuild.setBuildTargetId(m_buildTargetId);
    BuildDao(m_db->sqlDatabase()).addBuild(newBuild);
    m_builds.append(std::move(newBuild));
    endInsertRows();
}

int BuildsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_builds.size();
}

QVariant BuildsModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return {};

    const auto &build = m_builds.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::Name:
        return build.name();
    case Roles::BuildNumber:
        return build.id();
    case Roles::BuildTargetId:
        return build.buildTargetId();
    case Roles::Status:
        return build.status();
    case Roles::IconPath:
        return build.iconPath();
    case Roles::ArtifactName:
        return build.artifactName();
    case Roles::ArtifactSize:
        return build.artifactSize();
    case Roles::ArtifactPath:
        return build.artifactPath();
    case Roles::ManualDownload:
        return build.manualDownload();
    default:
        break;
    }

    return {};
}

bool BuildsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index))
        return false;

    auto &build = m_builds[index.row()];

    switch (role)
    {
    case Qt::EditRole:
    case Roles::Name:
        build.setName(value.toString());
        emit dataChanged(index, index, QVector<int>{ Qt::DisplayRole, Qt::EditRole, Roles::Name });
        break;
    case Roles::ManualDownload:
        build.setManualDownload(value.toBool());
        emit dataChanged(index, index, QVector<int>{ Roles::ManualDownload });
        break;
    default:
        return false;
    }

    BuildDao(m_db->sqlDatabase()).updateBuild(build);
    return true;
}

bool BuildsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count > m_builds.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    BuildDao dao(m_db->sqlDatabase());
    for (int i = row, end = row + count; i < end; ++i)
    {
        dao.removeBuild(m_builds.at(i));
    }

    m_builds.remove(row, count);
    return true;
}

QHash<int, QByteArray> BuildsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::BuildNumber] = "buildNumber";
    roles[Roles::BuildTargetId] = "buildTargetId";
    roles[Roles::Name] = "name";
    roles[Roles::Status] = "status";
    roles[Roles::IconPath] = "iconPath";
    roles[Roles::ArtifactName] = "artifactName";
    roles[Roles::ArtifactSize] = "artifactSize";
    roles[Roles::ArtifactPath] = "artifactPath";
    return roles;
}

Qt::ItemFlags BuildsModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return {};
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void BuildsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    auto buildTarget = BuildTargetDao(m_db->sqlDatabase()).buildTarget(m_buildTargetId);
    auto project = ProjectDao(m_db->sqlDatabase()).project(buildTarget.projectId());
    auto apiKey = ProfileDao(m_db->sqlDatabase()).getApiKey(project.profileId());
    auto *unityClient = new UnityApiClient(apiKey, this);

    connect(unityClient, &UnityApiClient::buildsFetched, unityClient, &UnityApiClient::deleteLater);
    connect(unityClient, &UnityApiClient::buildsFetched, this, &BuildsModel::onBuildsFetched);
    unityClient->fetchBuilds(project.organisationId(), project.cloudId(), buildTarget.cloudId());
}

void BuildsModel::onBuildsFetched(const QVector<Build> &builds)
{
    int count = m_builds.size();

    while (count--)
    {
        auto cloudId = m_builds.at(count).id();

        auto cloudBuildIt = std::find_if(
                    std::begin(builds),
                    std::end(builds),
                    [cloudId](const auto &project) -> bool { return project.id() == cloudId; });

        // remove projects that no longer exists
        if (cloudBuildIt == std::end(builds))
        {
            removeRow(count);
            continue;
        }
        else // update existing project
        {
            updateBuild(count, *cloudBuildIt);
        }
    }

    // add new projects
    for (const auto &cloudBuild : builds)
    {
        auto cloudId = cloudBuild.id();
        if (std::none_of(
                    std::begin(m_builds),
                    std::end(m_builds),
                    [cloudId](const auto &project) -> bool { return project.id() == cloudId; }))
        {
            addBuild(cloudBuild);
        }
    }
}

bool BuildsModel::isIndexValid(const QModelIndex &index) const
{
    return index.parent() == QModelIndex() && index.row() >= 0 && index.row() < rowCount() && index.column() == 0;
}

} // namespace ucd
