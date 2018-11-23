#ifndef UCD_DOWNLOADWORKER_H
#define UCD_DOWNLOADWORKER_H

#pragma once

#include "unityclouddownloader-core_global.h"
#include "build.h"

#include <atomic>

#include <QObject>

namespace ucd
{

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = nullptr);
    ~DownloadWorker() override = default;

    bool busy() const { return m_busy; }

    void download(const Build &build);

signals:
    void downloadCompleted(Build build);
    void downloadRequested(Build build);

private slots:
    void onDownloadRequested(Build build);

private:
    std::atomic_bool m_busy;
};

}

#endif // UCD_DOWNLOADWORKER_H
