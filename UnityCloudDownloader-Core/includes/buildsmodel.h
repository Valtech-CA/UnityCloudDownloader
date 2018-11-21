#ifndef UCD_BUILDSMODEL_H
#define UCD_BUILDSMODEL_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QAbstractListModel>
#include <QVector>
#include <QUuid>

namespace ucd
{

class Build;
class Database;

class UCD_SHARED_EXPORT BuildsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ucd::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QUuid buildTargetId READ buildTargetId WRITE setBuildTargetId NOTIFY buildTargetIdChanged)
public:
    enum Roles : int
    {
        BuildNumber = Qt::UserRole + 1,
        BuildTargetId,
        Name,
        Status,
        CreateTime,
        IconPath,
        ArtifactName,
        ArtifactSize,
        ArtifactPath,
        ManualDownload,
    };

    BuildsModel(QObject *parent = nullptr);
    virtual ~BuildsModel() override;

    Database* database() const { return m_db; }
    void setDatabase(Database *database);

    const QUuid& buildTargetId() const { return m_buildTargetId; }
    void setBuildTargetId(const QUuid &buildTargetId);

    bool updateBuild(int row, const Build &build);
    void addBuild(const Build &build);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void fetchMore(const QModelIndex &parent) override;

signals:
    void databaseChanged(Database *database);
    void buildTargetIdChanged(QUuid buildTargetId);

private slots:
    void onBuildsFetched(const QVector<Build> &builds);

private:
    bool isIndexValid(const QModelIndex &index) const;

    Database *m_db;
    QUuid m_buildTargetId;
    QVector<Build> m_builds;
};
}

#endif // UCD_BUILDSMODEL_H
