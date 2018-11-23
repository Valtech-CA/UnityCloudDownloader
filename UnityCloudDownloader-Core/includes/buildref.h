#ifndef UCD_BUILDREF_H
#define UCD_BUILDREF_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QObject>
#include <QUuid>

namespace ucd
{

class Build;

class UCD_SHARED_EXPORT BuildRef
{
    Q_GADGET
public:
    BuildRef();
    BuildRef(const Build &build);
    BuildRef(const BuildRef &) = default;
    BuildRef(BuildRef&&) noexcept = default;
    ~BuildRef() = default;

    BuildRef& operator=(const Build &build);
    BuildRef& operator=(const BuildRef&) = default;
    BuildRef& operator=(BuildRef&&) = default;

    bool operator==(const BuildRef &other) const;
    bool operator<(const BuildRef &other) const;

    operator Build() const;

private:
    QUuid m_buildTargetId;
    int m_buildNumber;
};

}

Q_DECLARE_METATYPE(ucd::BuildRef)

#endif // UCD_BUILDREF_H
