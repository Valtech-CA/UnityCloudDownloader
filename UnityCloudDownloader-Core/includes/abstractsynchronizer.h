#ifndef UCD_ABSTRACTSYNCHRONIZER_H
#define UCD_ABSTRACTSYNCHRONIZER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "isynchronizer.h"
#include "build.h"

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

signals:
    void downloadQueued(ucd::Build build);
    void downloadStarted(ucd::Build build);
    void downloadUpdated(ucd::Build build);
    void downloadCompleted(ucd::Build build);
    void downloadFailed(ucd::Build build);
    void synchronized();
};

}

#endif // UCD_ABSTRACTSYNCHRONIZER_H
