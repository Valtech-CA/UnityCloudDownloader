#ifndef UCD_DOWNLOADWORKER_H
#define UCD_DOWNLOADWORKER_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QObject>

namespace ucd
{

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = nullptr);

signals:

public slots:
};

}

#endif // UCD_DOWNLOADWORKER_H
