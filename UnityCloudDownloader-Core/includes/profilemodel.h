#ifndef UCD_PROFILEMODEL_H
#define UCD_PROFILEMODEL_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QAbstractListModel>
#include <QVector>

namespace ucd
{

class Profile;
class Database;

class UCD_SHARED_EXPORT ProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles : int
    {
        ProfileId = Qt::UserRole + 1,
        Name,
        RootPath,
        ApiKey,
    };

    explicit ProfilesModel(Database *data, QObject *parent = nullptr);
    virtual ~ProfilesModel() override;

    Q_INVOKABLE QModelIndex createProfile(const QString &name, const QString &apiKey, const QString &rootPath);
    QModelIndex addProfile(const Profile &profile);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    bool isIndexValid(const QModelIndex &index) const;

    Database *m_db;
    QVector<Profile> m_profiles;
};
}

#endif // UCD_PROFILEMODEL_H
