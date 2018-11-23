#ifndef UCD_DOWNLOADWORKER_H
#define UCD_DOWNLOADWORKER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "build.h"

#include <atomic>
#include <memory>

#include <QObject>
#include <QByteArray>
#include <QElapsedTimer>

class QNetworkAccessManager;
class QFile;
class QNetworkReply;

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
    void requestProgress();

signals:
    void downloadCompleted(ucd::Build build);
    void downloadFailed(ucd::Build build);
    void downloadRequested(ucd::Build build);
    void downloadUpdated(ucd::Build build, float ratio, qint64 speed);
    void progressRequested();

private slots:
    void onDownloadRequested(ucd::Build build);
    void onReadyRead();
    void onDownloadFinished();
    void onProgressRequested();

private:
    QUuid m_connectionId;
    std::atomic_bool m_busy;
    QNetworkAccessManager *m_network;
    Build m_build;
    QString m_storagePath;
    std::unique_ptr<QFile> m_outFile;
    QByteArray m_buffer;
    QNetworkReply *m_reply;
    QElapsedTimer m_progressTimer;
    qint64 m_lastSize;
};

}

#endif // UCD_DOWNLOADWORKER_H
