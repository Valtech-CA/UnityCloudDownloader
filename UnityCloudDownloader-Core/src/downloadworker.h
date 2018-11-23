#ifndef UCD_DOWNLOADWORKER_H
#define UCD_DOWNLOADWORKER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "build.h"

#include <atomic>
#include <memory>

#include <QObject>
#include <QByteArray>

class QNetworkAccessManager;
class QFile;

namespace ucd
{

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = nullptr);
    ~DownloadWorker() override;

    bool busy() const { return m_busy; }

    void download(const Build &build);

signals:
    void downloadCompleted(ucd::Build build);
    void downloadFailed(ucd::Build build);
    void downloadRequested(ucd::Build build);

private slots:
    void onDownloadRequested(ucd::Build build);
    void onReadyRead();
    void onDownloadFinished();

private:
    QUuid m_connectionId;
    std::atomic_bool m_busy;
    QNetworkAccessManager *m_network;
    Build m_build;
    QString m_storagePath;
    std::unique_ptr<QFile> m_outFile;
    QByteArray m_buffer;
};

}

#endif // UCD_DOWNLOADWORKER_H
