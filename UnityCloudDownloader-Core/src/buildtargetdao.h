#ifndef BUILDTARGETDAO_H
#define BUILDTARGETDAO_H

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
    BuildTargetDao(QSqlDatabase &&database);
    ~BuildTargetDao();

    void init();

    void addBuildTarget(const BuildTarget &buildTarget);
    void updateBuildTarget(const BuildTarget &buildTarget);
    void removeBuildTarget(const QUuid &buildTargetId);
    QVector<BuildTarget> buildTargets(const QUuid &projectId, bool includeBuilds = false);
    BuildTarget buildTarget(const QUuid &buildTargetId, bool includeBuilds = false);

    void removeBuildTargets(const QUuid &projectId);

private:
    QSqlDatabase m_db;
};

}

#endif // BUILDTARGETDAO_H
