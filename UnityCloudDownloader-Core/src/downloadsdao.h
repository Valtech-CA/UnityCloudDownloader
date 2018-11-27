#ifndef UCD_DOWNLOADSDAO_H
#define UCD_DOWNLOADSDAO_H

#pragma once

#include "buildref.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>

namespace ucd
{

class DownloadsDao
{
    enum Status : int
    {
        Unknown,
        Downloaded,
    };
public:
    DownloadsDao(const QSqlDatabase &database);
    ~DownloadsDao() = default;

    void init();

    QVector<BuildRef> downloadedBuilds(QUuid buildTargetId = {});
    void addDownload(BuildRef buildRef);
    void removeDownload(BuildRef buildRef);

private:
    QSqlDatabase m_db;
};

}

#endif // UCD_DOWNLOADSDAO_H
