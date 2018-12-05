#ifndef UCD_BUILDDAO_H
#define UCD_BUILDDAO_H

#pragma once

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class Build;
class BuildRef;

class BuildDao
{
public:
    BuildDao(const QSqlDatabase &database);
    ~BuildDao() = default;

    void init();

    bool hasBuild(const Build& build);
    bool hasBuild(const BuildRef& buildRef);

    void addBuild(const Build &build, bool orReplace = false);
    void updateBuild(const Build &build);
    void partialUpdate(const Build &build);
    void removeBuild(const Build &build);
    QVector<Build> builds(const QUuid &buildTargetId);
    Build build(const QUuid &buildTargetId, int buildNumber);

    void removeBuilds(const QUuid &buildTargetId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_BUILDDAO_H
