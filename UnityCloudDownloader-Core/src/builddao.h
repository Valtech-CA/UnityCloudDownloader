#ifndef UCD_BUILDDAO_H
#define UCD_BUILDDAO_H

#include <QVector>
#include <QSqlDatabase>

class QUuid;

namespace ucd
{

class Build;

class BuildDao
{
public:
    BuildDao(const QSqlDatabase &database);
    ~BuildDao() = default;

    void init();

    void addBuild(const Build &build);
    void updateBuild(const Build &build);
    void removeBuild(const Build &build);
    QVector<Build> builds(const QUuid &buildTargetId);

    void removeBuilds(const QUuid &buildTargetId);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_BUILDDAO_H
