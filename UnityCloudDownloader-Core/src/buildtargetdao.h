#ifndef BUILDTARGETDAO_H
#define BUILDTARGETDAO_H

class QSqlDatabase;

namespace ucd
{

class BuildTargetDao
{
public:
    BuildTargetDao(QSqlDatabase &database);

    void init();

private:
    QSqlDatabase &m_db;
};

}

#endif // BUILDTARGETDAO_H
