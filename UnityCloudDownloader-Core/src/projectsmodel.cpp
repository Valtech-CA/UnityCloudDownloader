#include "projectsmodel.h"

#include "project.h"
#include "projectdao.h"
#include "buildtargetdao.h"
#include "profiledao.h"
#include "database.h"
#include "unityapiclient.h"
#include "servicelocator.h"

#include <algorithm>

#include <QSqlDatabase>
#include <QTimerEvent>

namespace ucd
{

ProjectsModel::ProjectsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_updateTimer(0)
{
    m_updateTimer = startTimer(2000);
}

ProjectsModel::~ProjectsModel()
{
    killTimer(m_updateTimer);
}

void ProjectsModel::setProfileId(const QUuid &profileId)
{
    if (profileId == m_profileId)
        return;

    beginResetModel();
    m_profileId = profileId;
    if (!m_profileId.isNull())
    {
        m_projects = ProjectDao(ServiceLocator::database()).projects(m_profileId);
        fetchMore(QModelIndex());
    }
    else
    {
        m_projects.clear();
    }

    endResetModel();
    emit profileIdChanged(profileId);
}

bool ProjectsModel::updateProject(int row, const Project &project)
{
    if (row >= m_projects.size())
        return false;

    auto &currentProject = m_projects[row];

    if (currentProject.name() == project.name()
            && currentProject.iconPath() == project.iconPath())
        return true;

    currentProject.setName(project.name());
    currentProject.setIconPath(project.iconPath());

    ProjectDao(ServiceLocator::database()).updateProject(currentProject);
    emit dataChanged(index(row), index(row));
    return true;
}

void ProjectsModel::addProject(const Project &project)
{
    beginInsertRows(QModelIndex(), m_projects.size(), m_projects.size());
    Project newProject(project);
    newProject.setProfileId(m_profileId);
    ProjectDao(ServiceLocator::database()).addProject(newProject);
    m_projects.append(std::move(newProject));
    endInsertRows();
}

int ProjectsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_projects.count();
}

QVariant ProjectsModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return {};

    const auto &project = m_projects.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::Name:
        return project.name();
    case Roles::ProjectId:
        return project.id();
    case Roles::ProfileId:
        return project.profileId();
    case Roles::CloudId:
        return project.cloudId();
    case Roles::OrganisationId:
        return project.organisationId();
    case Roles::IconPath:
        return project.iconPath();
    case Roles::HasSynchedBuildTargets:
        return BuildTargetDao(ServiceLocator::database()).hasSynchedBuildTargets(project.id());
    default:
        break;
    }

    return {};
}

bool ProjectsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index))
        return false;

    auto &project = m_projects[index.row()];

    switch (role)
    {
    case Qt::EditRole:
    case Roles::Name:
        project.setName(value.toString());
        emit dataChanged(index, index, QVector<int>{ Qt::DisplayRole, Qt::EditRole, Roles::Name });
        break;
    case Roles::IconPath:
        project.setIconPath(value.toString());
        emit dataChanged(index, index, QVector<int>{ Roles::IconPath });
        break;
    default:
        return false;
    }

    ProjectDao(ServiceLocator::database()).updateProject(project);
    return true;
}

bool ProjectsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count > m_projects.count())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    ProjectDao dao(ServiceLocator::database());
    for (int i = row, end = row + count; i < end; ++i)
    {
        dao.removeProject(m_projects.at(i).id());
    }

    m_projects.remove(row, count);

    endRemoveRows();
    return true;
}

QHash<int, QByteArray> ProjectsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::Name] = "name";
    roles[Roles::ProjectId] = "id";
    roles[Roles::ProfileId] = "profileId";
    roles[Roles::CloudId] = "cloudId";
    roles[Roles::OrganisationId] = "orgId";
    roles[Roles::IconPath] = "iconPath";
    roles[Roles::HasSynchedBuildTargets] = "hasSynchedBuildTargets";
    return roles;
}

Qt::ItemFlags ProjectsModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return {};
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ProjectsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    auto apiKey = ProfileDao(ServiceLocator::database()).getApiKey(m_profileId);
    auto *unityClient = new UnityApiClient(apiKey, this);

    connect(unityClient, &UnityApiClient::projectsFetched, unityClient, &UnityApiClient::deleteLater);
    connect(unityClient, &UnityApiClient::projectsFetched, this, &ProjectsModel::onProjectsFetched);
    unityClient->fetchProjects();
}

void ProjectsModel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateTimer && !m_projects.isEmpty())
    {
        emit dataChanged(index(0), index(m_projects.size() - 1), QVector<int>{ Roles::HasSynchedBuildTargets });
    }
}

void ProjectsModel::onProjectsFetched(const QVector<Project> &projects)
{
    int count = m_projects.size();

    while (count--)
    {
        auto cloudId = m_projects.at(count).cloudId();

        auto cloudProjectIt = std::find_if(
                    std::begin(projects),
                    std::end(projects),
                    [cloudId](const auto &project) -> bool { return project.cloudId() == cloudId; });

        // remove projects that no longer exists
        if (cloudProjectIt == std::end(projects))
        {
            removeRow(count);
            continue;
        }
        else // update existing project
        {
            updateProject(count, *cloudProjectIt);
        }
    }

    // add new projects
    for (const auto &cloudProject : projects)
    {
        auto cloudId = cloudProject.cloudId();
        if (std::none_of(
                    std::begin(m_projects),
                    std::end(m_projects),
                    [cloudId](const auto &project) -> bool { return project.cloudId() == cloudId; }))
        {
            addProject(cloudProject);
        }
    }
}

bool ProjectsModel::isIndexValid(const QModelIndex &index) const
{
    return index.parent() == QModelIndex() && index.row() >= 0 && index.row() < rowCount() && index.column() == 0;
}

} // namespace ucd
