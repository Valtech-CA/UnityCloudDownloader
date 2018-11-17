#include "projectsmodel.h"

#include "project.h"
#include "projectdao.h"
#include "profiledao.h"
#include "database.h"
#include "unityapiclient.h"

#include <QSqlDatabase>

namespace ucd
{

ProjectsModel::ProjectsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_hasSynced(false)
{}

ProjectsModel::~ProjectsModel()
{}

void ProjectsModel::setDatabase(Database *database)
{
    if (database == m_db)
        return;

    beginResetModel();
    m_db = database;
    if (m_db != nullptr && !m_profileId.isNull())
    {
        m_projects = ProjectDao(m_db->sqlDatabase()).projects();
    }
    else
    {
        m_projects.clear();
    }

    endResetModel();
    emit databaseChanged(database);
}

void ProjectsModel::setProfileId(const QUuid &profileId)
{
    if (profileId == m_profileId)
        return;

    beginResetModel();
    m_profileId = profileId;
    if (m_db != nullptr && !m_profileId.isNull())
    {
        m_projects = ProjectDao(m_db->sqlDatabase()).projects();
    }
    else
    {
        m_projects.clear();
    }

    endResetModel();
    emit profileIdChanged(profileId);
}

int ProjectsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_projects.count();
}

QVariant ProjectsModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

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
    default:
        break;
    }

    return QVariant();
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
        break;
    case Roles::IconPath:
        project.setIconPath(value.toString());
        break;
    default:
        return false;
    }

    ProjectDao(m_db->sqlDatabase()).updateProject(project);
    return true;
}

bool ProjectsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row + count > m_projects.count())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count -1);

    ProjectDao dao(m_db->sqlDatabase());
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
    return roles;
}

Qt::ItemFlags ProjectsModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return Qt::ItemFlags();
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ProjectsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    auto *unityClient = new UnityApiClient(this);

    auto apiKey = ProfileDao(m_db->sqlDatabase()).getApiKey(m_profileId);

    connect(unityClient, &UnityApiClient::projectsFetched, unityClient, &UnityApiClient::deleteLater);
    connect(unityClient, &UnityApiClient::projectsFetched, this, &ProjectsModel::onProjectsFetched);
    unityClient->fetchProjects(apiKey);
    m_hasSynced = true;
}

bool ProjectsModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_hasSynced;
}

void ProjectsModel::onProjectsFetched(const QVector<Project> &projects)
{
    beginResetModel();
    m_projects = projects;
    endResetModel();
}

bool ProjectsModel::isIndexValid(const QModelIndex &index) const
{
    return index.parent() == QModelIndex() && index.row() >= 0 && index.row() < rowCount() && index.column() == 0;
}

} // namespace ucd
