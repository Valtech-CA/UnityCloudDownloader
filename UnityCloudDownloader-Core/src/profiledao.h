#ifndef UCD_PROFILEDAO_H
#define UCD_PROFILEDAO_H

class QSqlDatabase;

namespace ucd
{

class ProfileDao
{
public:
    ProfileDao(QSqlDatabase &database);

    void init();

private:
    QSqlDatabase &m_db;
};

}

#endif // UCD_PROFILEDAO_H
