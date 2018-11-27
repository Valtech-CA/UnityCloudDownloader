#ifndef UCD_BUILDREF_H
#define UCD_BUILDREF_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QObject>
#include <QUuid>

class QDebug;

namespace ucd
{
class BuildRef;
}

QDebug &operator<<(QDebug &out, const ucd::BuildRef &value);
QDataStream &operator<<(QDataStream &out, const ucd::BuildRef &value);
QDataStream &operator>>(QDataStream &in, ucd::BuildRef &dest);

namespace ucd
{

class Build;

class UCD_SHARED_EXPORT BuildRef
{
    Q_GADGET
    friend QDebug &::operator<<(QDebug &out, const ucd::BuildRef &value);
    friend QDataStream &::operator<<(QDataStream &out, const ucd::BuildRef &value);
    friend QDataStream &::operator>>(QDataStream &in, ucd::BuildRef &dest);

public:
    BuildRef();
    BuildRef(const Build &build);
    BuildRef(QUuid buildTargetId, int buildNumber);
    BuildRef(const BuildRef &) = default;
    BuildRef(BuildRef&&) noexcept = default;
    ~BuildRef() = default;

    BuildRef& operator=(const Build &build);
    BuildRef& operator=(const BuildRef&) = default;
    BuildRef& operator=(BuildRef&&) = default;

    bool operator==(const BuildRef &other) const;
    bool operator<(const BuildRef &other) const;

    operator Build() const;

    QUuid buildTargetId() const { return m_buildTargetId; }
    int buildNumber() const { return m_buildNumber; }

private:
    QUuid m_buildTargetId;
    int m_buildNumber;
};

}

Q_DECLARE_METATYPE(ucd::BuildRef)

#endif // UCD_BUILDREF_H
