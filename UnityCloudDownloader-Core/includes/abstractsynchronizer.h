#ifndef UCD_ABSTRACTSYNCHRONIZER_H
#define UCD_ABSTRACTSYNCHRONIZER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "isynchronizer.h"

#include <QObject>

namespace ucd
{

class Database;

class UCD_SHARED_EXPORT AbstractSynchronizer : public QObject, public ISynchronizer
{
    Q_OBJECT
public:
    AbstractSynchronizer(QObject *parent = nullptr);
    AbstractSynchronizer(const AbstractSynchronizer&) = delete;
    virtual ~AbstractSynchronizer() override = default;

    AbstractSynchronizer& operator=(const AbstractSynchronizer&) = delete;
};

}

#endif // UCD_ABSTRACTSYNCHRONIZER_H
