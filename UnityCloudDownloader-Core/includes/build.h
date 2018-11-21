#ifndef UCD_BUILD_H
#define UCD_BUILD_H

#pragma once

#include "unityclouddownloader-core_global.h"

#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QDateTime>

namespace ucd
{

class UCD_SHARED_EXPORT Build
{
    Q_GADGET
public:
    enum Status : char
    {
        Unknown,
        Queued,
        SentToBuilder,
        Started,
        Restarted,
        Success,
        Failure,
        Canceled,
    };
    Q_ENUM(Status)

    static Status statusFromString(const QString &value);

    Build();
    Build(Build &&other) noexcept = default;
    Build(const Build& other) = default;
    ~Build() = default;

    Build& operator=(Build &&other) noexcept = default;
    Build& operator=(const Build &other) = default;

    bool isLike(const Build &other);
    void takeFrom(const Build &other);

    const QString& name() const { return m_name; }
    int id() const { return m_buildNumber; }
    Status status() const { return m_status; }
    const QUuid& buildTargetId() const { return m_buildTargetId; }
    const QDateTime createTime() const { return m_createTime; }
    const QString& iconPath() const { return m_iconPath; }
    const QString& artifactName() const { return m_artifactName; }
    uint64_t artifactSize() const { return m_artifactSize; }
    const QString& artifactPath() const { return m_artifactPath; }
    bool manualDownload() const { return m_manual; }

    void setName(QString name);
    void setId(int id);
    void setStatus(int status);
    void setBuildTargetId(const QUuid &buildTargetId);
    void setCreateTime(const QDateTime &createTime);
    void setIconPath(QString iconPath);
    void setArtifactName(QString artifactName);
    void setArtifactSize(uint64_t size);
    void setArtifactPath(QString artifactPath);
    void setManualDownload(bool value);

private:
    int m_buildNumber;
    Status m_status;
    QUuid m_buildTargetId;
    QString m_name;
    QDateTime m_createTime;
    QString m_iconPath;
    QString m_artifactName;
    uint64_t m_artifactSize;
    QString m_artifactPath;
    bool m_manual;
};

}

#endif // UCD_BUILD_H
