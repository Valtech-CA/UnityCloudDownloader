#include "projectsmodel.h"

namespace ucd
{

ProjectsModel::ProjectsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ProjectsModel::~ProjectsModel()
{
}

void ProjectsModel::setDatabase(Database *database)
{
    if (database == m_db)
        return;

    beginResetModel();
    m_db = database;
    if (m_db != nullptr && !m_profileId.isNull())
    {

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

}

bool ProjectsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

bool ProjectsModel::removeRows(int row, int count, const QModelIndex &parent)
{

}

QHash<int, QByteArray> ProjectsModel::roleNames() const
{

}

Qt::ItemFlags ProjectsModel::flags(const QModelIndex &index) const
{

}

bool ProjectsModel::isIndexValid(const QModelIndex &index) const
{

}

} // namespace ucd
