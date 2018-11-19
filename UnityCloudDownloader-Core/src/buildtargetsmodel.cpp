#include "buildtargetsmodel.h"

#include "buildtarget.h"
#include "buildtargetdao.h"
#include "projectdao.h"
#include "profiledao.h"
#include "database.h"
#include "unityapiclient.h"

#include <algorithm>

#include <QSqlDatabase>

namespace ucd
{

BuildTargetsModel::BuildTargetsModel(QObject *parent)
        : QAbstractListModel(parent)
{}

BuildTargetsModel::~BuildTargetsModel()
{}

void BuildTargetsModel::setDatabase(Database *database)
{
    if (database == m_db)
        return;

    beginResetModel();
    m_db = database;
    if (m_db != nullptr && !m_projectId.isNull())
    {
        m_buildTargets = BuildTargetDao(m_db->sqlDatabase()).buildTargets(m_projectId);
    }
    else
    {
        m_buildTargets.clear();
    }

    endResetModel();
    emit databaseChanged(database);
}

void BuildTargetsModel::setProjectId(const QUuid &projectId)
{
    if (projectId == m_projectId)
        return;

    beginResetModel();
    m_projectId = projectId;
    if (m_db != nullptr && !m_projectId.isNull())
    {
        m_buildTargets = BuildTargetDao(m_db->sqlDatabase()).buildTargets(m_projectId);
    }
    else
    {
        m_buildTargets.clear();
    }

    endResetModel();
    emit projectIdChanged(projectId);
}

bool BuildTargetsModel::updateBuildTarget(int row, const BuildTarget &buildTarget)
{
    if (row >= m_buildTargets.size())
        return false;

    auto &currentBuildTarget = m_buildTargets[row];

    if (currentBuildTarget.name() == buildTarget.name()
        && currentBuildTarget.platform() == buildTarget.platform())
        return true;

    currentBuildTarget.setName(buildTarget.name());
    currentBuildTarget.setPlatform(buildTarget.platform());

    BuildTargetDao(m_db->sqlDatabase()).updateBuildTarget(currentBuildTarget);
    emit dataChanged(index(row), index(row));
    return true;
}

void BuildTargetsModel::addBuildTarget(const BuildTarget &buildTarget)
{
    beginInsertRows(QModelIndex(), m_buildTargets.size(), m_buildTargets.size());
    BuildTarget newBuildTarget(buildTarget);
    newBuildTarget.setProjectId(m_projectId);
    BuildTargetDao(m_db->sqlDatabase()).addBuildTarget(newBuildTarget);
    m_buildTargets.append(std::move(newBuildTarget));
    endInsertRows();
}

int BuildTargetsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_buildTargets.size();
}

QVariant BuildTargetsModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

    const auto &buildTarget = m_buildTargets.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::Name:
        return buildTarget.name();
    case Roles::BuildTargetId:
        return buildTarget.id();
    case Roles::ProjectId:
        return buildTarget.projectId();
    case Roles::CloudId:
        return buildTarget.cloudId();
    case Roles::Platform:
        return buildTarget.platform();
    default:
        break;
    }

    return QVariant();
}

bool BuildTargetsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index))
        return false;

    auto &buildTarget = m_buildTargets[index.row()];

    switch (role)
    {
    case Qt::EditRole:
    case Roles::Name:
        buildTarget.setName(value.toString());
        emit dataChanged(index, index, QVector<int>{ Qt::DisplayRole, Qt::EditRole, Roles::Name });
        break;
    case Roles::Platform:
        buildTarget.setPlatform(value.toString());
        emit dataChanged(index, index, QVector<int>{ Roles::Platform });
        break;
    default:
        return false;
    }

    BuildTargetDao(m_db->sqlDatabase()).updateBuildTarget(buildTarget);
    return true;
}

bool BuildTargetsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count > m_buildTargets.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    BuildTargetDao dao(m_db->sqlDatabase());
    for (int i = row, end = row + count; i < end; ++i)
    {
        dao.removeBuildTarget(m_buildTargets.at(i).id());
    }

    m_buildTargets.remove(row, count);

    endRemoveRows();
    return true;
}

QHash<int, QByteArray> BuildTargetsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::Name] = "name";
    roles[Roles::BuildTargetId] = "buildTargetId";
    roles[Roles::ProjectId] = "projectId";
    roles[Roles::CloudId] = "cloudId";
    roles[Roles::Platform] = "platform";
    return roles;
}

Qt::ItemFlags BuildTargetsModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void BuildTargetsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    auto project = ProjectDao(m_db->sqlDatabase()).project(m_projectId);
    auto apiKey = ProfileDao(m_db->sqlDatabase()).getApiKey(project.profileId());
    auto *unityClient = new UnityApiClient(apiKey, this);

    connect(unityClient, &UnityApiClient::buildTargetsFetched, unityClient, &UnityApiClient::deleteLater);
    connect(unityClient, &UnityApiClient::buildTargetsFetched, this, &BuildTargetsModel::onBuildTargetsFetched);
    unityClient->fetchBuildTargets(project.organisationId(), project.cloudId());
    m_hasSynced = true;
}

bool BuildTargetsModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_hasSynced;
}

void BuildTargetsModel::onBuildTargetsFetched(const QVector<BuildTarget> &buildTargets)
{
    int count = m_buildTargets.size();

    while (count--)
    {
        auto cloudId = m_buildTargets.at(count).cloudId();

        auto cloudBuildTargtIt = std::find_if(
                                         std::begin(buildTargets),
                                         std::end(buildTargets),
                                         [cloudId](const auto &buildTarget) -> bool
        {
            return buildTarget.cloudId() == cloudId;
        });

        // remove build targets that no longer exists
        if (cloudBuildTargtIt == std::end(buildTargets))
        {
            removeRow(count);
            continue;
        }
        else // update existing build target
        {
            updateBuildTarget(count, *cloudBuildTargtIt);
        }
    }

    // add new projects
    for (const auto &cloudBuildTargt : buildTargets)
    {
        auto cloudId = cloudBuildTargt.cloudId();
        if (std::none_of(
                    std::begin(m_buildTargets),
                    std::end(m_buildTargets),
                    [cloudId](const auto &buildTarget) -> bool { return buildTarget.cloudId() == cloudId; }))
        {
            addBuildTarget(cloudBuildTargt);
        }
    }
}

bool BuildTargetsModel::isIndexValid(const QModelIndex &index) const
{
    return index.parent() == QModelIndex() && index.row() >= 0 && index.row() < rowCount() && index.column() == 0;
}

} // namespace ucd
