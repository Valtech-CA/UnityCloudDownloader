#ifndef UCD_PROJECTDAO_H
#define UCD_PROJECTDAO_H

class QSqlDatabase;

namespace ucd
{

class ProjectDao
{
public:
    ProjectDao(QSqlDatabase &database);

    void init();

private:
    QSqlDatabase &m_db;
};

}

#endif // UCD_PROJECTDAO_H
