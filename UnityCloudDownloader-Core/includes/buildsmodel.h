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

class UCD_SHARED_EXPORT BuildsModel : public QAbstractListModel
{
    Q_OBJECT
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
        BuildRef,
    };

    BuildsModel(QObject *parent = nullptr);
    virtual ~BuildsModel() override;

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
    void buildTargetIdChanged(QUuid buildTargetId);

private slots:
    void onBuildsFetched(const QVector<Build> &builds);

private:
    bool isIndexValid(const QModelIndex &index) const;

    QUuid m_buildTargetId;
    QVector<Build> m_builds;
};
}

#endif // UCD_BUILDSMODEL_H
