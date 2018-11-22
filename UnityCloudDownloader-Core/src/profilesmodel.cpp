#include "profilesmodel.h"

#include "profile.h"
#include "profiledao.h"
#include "database.h"
#include "servicelocator.h"

#include <QSqlDatabase>

namespace ucd
{

ProfilesModel::ProfilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_profiles = ProfileDao(ServiceLocator::database()).profiles();
}

ProfilesModel::~ProfilesModel()
{}

QModelIndex ProfilesModel::createProfile(const QString &name, const QString &apiKey, const QString &rootPath)
{
    Profile newProfile;
    newProfile.setName(name);
    newProfile.setApiKey(apiKey);
    newProfile.setRootPath(rootPath);
    return addProfile(newProfile);
}

QModelIndex ProfilesModel::addProfile(const Profile &profile)
{
    beginInsertRows(QModelIndex(), m_profiles.count(), m_profiles.count());
    ProfileDao(ServiceLocator::database()).addProfile(profile);
    m_profiles.append(profile);
    endInsertRows();
    return index(m_profiles.count() - 1);
}

bool ProfilesModel::remove(int index, int count)
{
    return removeRows(index, count, QModelIndex());
}

int ProfilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_profiles.count();
}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

    const auto &profile = m_profiles.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::Name:
        return profile.name();
    case Roles::ApiKey:
        return profile.apiKey();
    case Roles::ProfileId:
        return profile.uuid();
    case Roles::RootPath:
        return  profile.rootPath();
    default:
        break;
    }

    return QVariant();
}

bool ProfilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index))
        return false;

    auto &profile = m_profiles[index.row()];

    switch (role)
    {
    case Qt::EditRole:
    case Roles::Name:
        profile.setName(value.toString());
        break;
    case Roles::ApiKey:
        profile.setApiKey(value.toString());
        break;
    case Roles::RootPath:
        profile.setRootPath(value.toString());
        break;
    default:
        return false;
    }

    ProfileDao(ServiceLocator::database()).updateProfile(profile);
    return true;
}

bool ProfilesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row + count > m_profiles.count())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    ProfileDao dao(ServiceLocator::database());
    for (int i = row, end = row + count; i < end; ++i)
    {
        dao.removeProfile(m_profiles.at(i).uuid());
    }

    m_profiles.remove(row, count);

    endRemoveRows();
    return true;
}

QHash<int, QByteArray> ProfilesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::Name] = "name";
    roles[Roles::ApiKey] = "apiKey";
    roles[Roles::RootPath] = "rootPath";
    roles[Roles::ProfileId] = "id";
    return roles;
}

Qt::ItemFlags ProfilesModel::flags(const QModelIndex &index) const
{
    if (!isIndexValid(index))
        return {};
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool ProfilesModel::isIndexValid(const QModelIndex &index) const
{
    return index.parent() == QModelIndex() && index.row() >= 0 && index.row() < rowCount() && index.column() == 0;
}

} // namespace ucd
