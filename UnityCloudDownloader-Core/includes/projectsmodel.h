#ifndef UCD_PROJECTSMODEL_H
#define UCD_PROJECTSMODEL_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QAbstractListModel>
#include <QVector>
#include <QUuid>

namespace ucd
{

class Project;

class UCD_SHARED_EXPORT ProjectsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QUuid profileId READ profileId WRITE setProfileId NOTIFY profileIdChanged)
public:
    enum Roles : int
    {
        ProjectId = Qt::UserRole + 1,
        ProfileId,
        CloudId,
        Name,
        OrganisationId,
        IconPath,
    };

    ProjectsModel(QObject *parent = nullptr);
    virtual ~ProjectsModel() override;

    QUuid profileId() const { return m_profileId; }
    void setProfileId(const QUuid &profileId);

    bool updateProject(int row, const Project &project);
    void addProject(const Project &project);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void fetchMore(const QModelIndex &parent) override;

signals:
    void profileIdChanged(QUuid profileId);

private slots:
    void onProjectsFetched(const QVector<Project> &projects);

private:
    bool isIndexValid(const QModelIndex &index) const;

    QUuid m_profileId;
    QVector<Project> m_projects;
};

}

#endif // UCD_PROJECTSMODEL_H
