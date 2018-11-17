#ifndef UDC_BUILDTARGET_H
#define UDC_BUILDTARGET_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QMetaType>
#include <QString>
#include <QUuid>

namespace ucd
{

class UCD_SHARED_EXPORT BuildTarget
{
public:
    BuildTarget();
    BuildTarget(const BuildTarget &other);
    BuildTarget(BuildTarget &&other);
    ~BuildTarget();

    BuildTarget& operator=(const BuildTarget &other);
    BuildTarget& operator=(BuildTarget &&other);

    const QString& name() const { return m_name; }
    const QUuid& id() const { return m_id; }
    const QUuid& projectId() const { return m_projectId; }
    const QString& cloudId() const { return m_cloudId; }
    const QString& platform() const { return m_platform; }

    void setName(const QString &name);
    void setId(const QUuid &id);
    void setProjectId(const QUuid &projectId);
    void setCloudId(const QString &cloudId);
    void setPlatform(const QString &platform);

private:
    QUuid m_id;
    QUuid m_projectId;
    QString m_cloudId;
    QString m_name;
    QString m_platform;
};

}

QDataStream &operator<<(QDataStream &out, const ucd::BuildTarget &value);
QDataStream &operator>>(QDataStream &in, ucd::BuildTarget &dest);

Q_DECLARE_METATYPE(ucd::BuildTarget)

#endif // UDC_BUILDTARGET_H
