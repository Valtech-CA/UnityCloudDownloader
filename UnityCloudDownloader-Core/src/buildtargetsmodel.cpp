#include "buildtargetsmodel.h"

#include "buildtarget.h"
#include "buildtargetdao.h"
#include "projectdao.h"
#include "profiledao.h"
#include "database.h"
#include "unityapiclient.h"
#include "servicelocator.h"

#include <algorithm>

#include <QSqlDatabase>

namespace ucd
{

BuildTargetsModel::BuildTargetsModel(QObject *parent)
        : QAbstractListModel(parent)
{}

BuildTargetsModel::~BuildTargetsModel()
{}

void BuildTargetsModel::setProjectId(const QUuid &projectId)
{
    if (projectId == m_projectId)
        return;

    beginResetModel();
    m_projectId = projectId;
    if (!m_projectId.isNull())
    {
        m_buildTargets = BuildTargetDao(ServiceLocator::database()).buildTargets(m_projectId);
        fetchMore(QModelIndex());
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

    BuildTargetDao(ServiceLocator::database()).updateBuildTarget(currentBuildTarget);
    emit dataChanged(index(row), index(row));
    return true;
}

void BuildTargetsModel::addBuildTarget(const BuildTarget &buildTarget)
{
    beginInsertRows(QModelIndex(), m_buildTargets.size(), m_buildTargets.size());
    BuildTarget newBuildTarget(buildTarget);
    newBuildTarget.setProjectId(m_projectId);
    BuildTargetDao(ServiceLocator::database()).addBuildTarget(newBuildTarget);
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
    case Roles::Synchronize:
        return buildTarget.sync();
    case Roles::MinBuilds:
        return buildTarget.minBuilds();
    case Roles::MaxBuilds:
        return buildTarget.maxBuilds();
    case Roles::MaxDaysOld:
        return buildTarget.maxDaysOld();
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
    case Roles::Synchronize:
        buildTarget.setSync(value.toBool());
        emit dataChanged(index, index, QVector<int>{ Roles::Synchronize });
        break;
    case Roles::MinBuilds:
        buildTarget.setMinBuilds(value.toInt());
        emit dataChanged(index, index, QVector<int>{ Roles::MinBuilds });
        break;
    case Roles::MaxBuilds:
        buildTarget.setMaxBuilds(value.toInt());
        emit dataChanged(index, index, QVector<int>{ Roles::MaxBuilds });
        break;
    case Roles::MaxDaysOld:
        buildTarget.setMaxDaysOld(value.toInt());
        emit dataChanged(index, index, QVector<int>{ Roles::MaxDaysOld });
        break;
    default:
        return false;
    }

    BuildTargetDao(ServiceLocator::database()).updateBuildTarget(buildTarget);
    return true;
}

bool BuildTargetsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count > m_buildTargets.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    BuildTargetDao dao(ServiceLocator::database());
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
    roles[Roles::Synchronize] = "sync";
    roles[Roles::MinBuilds] = "minBuilds";
    roles[Roles::MaxBuilds] = "maxBuilds";
    roles[Roles::MaxDaysOld] = "maxDaysOld";
    return roles;
}

Qt::ItemFlags BuildTargetsModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return {};
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void BuildTargetsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    auto project = ProjectDao(ServiceLocator::database()).project(m_projectId);
    auto apiKey = ProfileDao(ServiceLocator::database()).getApiKey(project.profileId());
    auto *unityClient = new UnityApiClient(apiKey, this);

    connect(unityClient, &UnityApiClient::buildTargetsFetched, unityClient, &UnityApiClient::deleteLater);
    connect(unityClient, &UnityApiClient::buildTargetsFetched, this, &BuildTargetsModel::onBuildTargetsFetched);
    unityClient->fetchBuildTargets(project.organisationId(), project.cloudId());
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
