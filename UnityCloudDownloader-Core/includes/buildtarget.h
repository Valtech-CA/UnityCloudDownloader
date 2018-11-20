#ifndef UDC_BUILDTARGET_H
#define UDC_BUILDTARGET_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "build.h"

#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QVector>

namespace ucd
{

typedef QVector<Build> BuildList;

class UCD_SHARED_EXPORT BuildTarget
{
public:
    BuildTarget();
    BuildTarget(const BuildTarget &other) = default;
    BuildTarget(BuildTarget &&other) noexcept;
    ~BuildTarget() = default;

    BuildTarget& operator=(const BuildTarget &other);
    BuildTarget& operator=(BuildTarget &&other) noexcept;

    const QString& name() const { return m_name; }
    const QUuid& id() const { return m_id; }
    const QUuid& projectId() const { return m_projectId; }
    const QString& cloudId() const { return m_cloudId; }
    const QString& platform() const { return m_platform; }
    bool sync() const { return m_sync; }
    int minBuilds() const { return m_minBuilds; }
    int maxBuilds() const { return m_maxBuilds; }
    int maxDaysOld() const { return m_maxDaysOld; }
    const BuildList& builds() const { return m_builds; }

    void setName(const QString &name);
    void setId(const QUuid &id);
    void setProjectId(const QUuid &projectId);
    void setCloudId(const QString &cloudId);
    void setPlatform(const QString &platform);
    void setSync(bool value);
    void setMinBuilds(int value);
    void setMaxBuilds(int value);
    void setMaxDaysOld(int value);
    void setBuilds(const BuildList &builds);

private:
    QUuid m_id;
    QUuid m_projectId;
    QString m_cloudId;
    QString m_name;
    QString m_platform;
    bool m_sync;
    int m_minBuilds;
    int m_maxBuilds;
    int m_maxDaysOld;
    BuildList m_builds;
};

}

QDataStream &operator<<(QDataStream &out, const ucd::BuildTarget &value);
QDataStream &operator>>(QDataStream &in, ucd::BuildTarget &dest);

Q_DECLARE_METATYPE(ucd::BuildTarget)

#endif // UDC_BUILDTARGET_H
