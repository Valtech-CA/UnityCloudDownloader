#ifndef UCD_PROJECT_H
#define UCD_PROJECT_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "buildtarget.h"

#include <QString>
#include <QVector>

namespace ucd
{

typedef QVector<BuildTarget> BuildTargetList;

class UCD_SHARED_EXPORT Project
{
public:
    Project();
    explicit Project(const Project &other);
    explicit Project(Project &&other);
    ~Project();

    Project& operator=(const Project &other);
    Project& operator=(Project &&other);

    const QString& name() const { return m_name; }
    const QString& id() const { return m_id; }
    const QString& organisationId() const { return m_orgId; }
    const QString& iconPath() const { return m_iconPath; }
    const BuildTargetList& buildTargets() const { return m_buildTargets; }

    void setName(const QString &name);
    void setId(const QString &id);
    void setOrganisationId(const QString &id);
    void setIconPath(const QString &iconPath);
    void setBuildTargets(const BuildTargetList &buildTargets);

private:
    QString m_name;
    QString m_id;
    QString m_orgId;
    QString m_iconPath;
    BuildTargetList m_buildTargets;
};

}

QDataStream &operator<<(QDataStream &out, const ucd::BuildTargetList &value);
QDataStream &operator>>(QDataStream &in, ucd::BuildTargetList &dest);

QDataStream &operator<<(QDataStream &out, const ucd::Project &value);
QDataStream &operator>>(QDataStream &in, ucd::Project &dest);

Q_DECLARE_METATYPE(ucd::BuildTargetList)
Q_DECLARE_METATYPE(ucd::Project)

#endif // UCD_PROJECT_H
