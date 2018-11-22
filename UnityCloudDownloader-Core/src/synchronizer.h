#ifndef UCD_SYNCHRONIZER_H
#define UCD_SYNCHRONIZER_H

#include "abstractsynchronizer.h"

namespace ucd
{

class Database;

class Synchronizer : public AbstractSynchronizer
{
    Q_OBJECT
public:
    Synchronizer(QObject *parent = nullptr);
    ~Synchronizer() override = default;

    void manualDownload(const Build &build) override;
};

}

#endif // UCD_SYNCHRONIZER_H
