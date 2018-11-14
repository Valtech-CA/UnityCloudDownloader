#include "profilemodel.h"

#include "profile.h"
#include "profiledao.h"
#include "database.h"

namespace ucd
{

ProfilesModel::ProfilesModel(Database *data, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(data)
{
}

ProfilesModel::~ProfilesModel()
{}

QModelIndex ProfilesModel::addProfile(const Profile &profile)
{

}

int ProfilesModel::rowCount(const QModelIndex &parent) const
{

}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const
{

}

bool ProfilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

bool ProfilesModel::removeRows(int row, int count, const QModelIndex &parent)
{

}

QHash<int, QByteArray> ProfilesModel::roleNames() const
{

}

} // namespace ucd
