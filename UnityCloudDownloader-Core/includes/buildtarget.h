#ifndef UDC_BUILDTARGET_H
#define UDC_BUILDTARGET_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QMetaType>
#include <QString>

namespace ucd
{

class UCD_SHARED_EXPORT BuildTarget
{
public:
    BuildTarget();
    explicit BuildTarget(const BuildTarget &other);
    explicit BuildTarget(BuildTarget &&other);
    ~BuildTarget();

    BuildTarget& operator=(const BuildTarget &other);
    BuildTarget& operator=(BuildTarget &&other);

    const QString& name() const { return m_name; }
    const QString& id() const { return m_id; }
    const QString& profileId() const { return m_profileId; }
    const QString& projectId() const { return m_projectId; }
    const QString& platform() const { return m_platform; }

    void setName(const QString &name);
    void setId(const QString &id);
    void setProfileId(const QString &profileId);
    void setProjectId(const QString &projectId);
    void setPlatform(const QString &platform);

private:
    QString m_id;
    QString m_profileId;
    QString m_projectId;
    QString m_name;
    QString m_platform;
};

}

QDataStream &operator<<(QDataStream &out, const ucd::BuildTarget &value);
QDataStream &operator>>(QDataStream &in, ucd::BuildTarget &dest);

Q_DECLARE_METATYPE(ucd::BuildTarget)

#endif // UDC_BUILDTARGET_H
