#ifndef UCD_BUILDTARGETSMODEL_H
#define UCD_BUILDTARGETSMODEL_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QAbstractListModel>
#include <QVector>
#include <QUuid>

namespace ucd
{

class BuildTarget;
class Database;

class UCD_SHARED_EXPORT BuildTargetsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ucd::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QUuid projectId READ projectId WRITE setProjectId NOTIFY projectIdChanged)
public:
    enum Roles : int
    {
        BuildTargetId = Qt::UserRole + 1,
        ProjectId,
        CloudId,
        Name,
        Platform,
        Synchronize,
        MinBuilds,
        MaxBuilds,
        MaxDaysOld,
    };

    BuildTargetsModel(QObject *parent = nullptr);
    virtual ~BuildTargetsModel() override;

    Database* database() const { return m_db; }
    void setDatabase(Database *database);

    QUuid projectId() const { return m_projectId; }
    void setProjectId(const QUuid &projectId);

    bool updateBuildTarget(int row, const BuildTarget &buildTarget);
    void addBuildTarget(const BuildTarget &buildTarget);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

signals:
    void databaseChanged(Database *database);
    void projectIdChanged(QUuid projectId);

private slots:
    void onBuildTargetsFetched(const QVector<BuildTarget> &buildTargets);

private:
    bool isIndexValid(const QModelIndex &index) const;

    Database *m_db;
    QUuid m_projectId;
    QVector<BuildTarget> m_buildTargets;
    bool m_hasSynced;
};

}

#endif // UCD_BUILDTARGETSMODEL_H
