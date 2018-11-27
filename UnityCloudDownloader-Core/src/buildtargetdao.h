#ifndef UCD_BUILDTARGETDAO_H
#define UCD_BUILDTARGETDAO_H

#pragma once

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class BuildTarget;

class BuildTargetDao
{
public:
    BuildTargetDao(const QSqlDatabase &database);
    ~BuildTargetDao() = default;

    void init();

    void addBuildTarget(const BuildTarget &buildTarget);
    void updateBuildTarget(const BuildTarget &buildTarget);
    void removeBuildTarget(const QUuid &buildTargetId);
    QVector<BuildTarget> buildTargets(const QUuid &projectId, bool includeBuilds = false);
    BuildTarget buildTarget(const QUuid &buildTargetId, bool includeBuilds = false);

    bool hasSynchedBuildTargets(const QUuid &projectId);

    void removeBuildTargets(const QUuid &projectId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_BUILDTARGETDAO_H
