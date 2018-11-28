#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#pragma once

#include <QObject>

class QFile;

class LogManagerGuard;

class LogManager : public QObject
{
    Q_OBJECT
    friend class LogManagerGuard;
public:
    static LogManager* instance() { return s_instance; }

    explicit LogManager(const QString &storagePath, QObject *parent = nullptr);
    ~LogManager() override;

    void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &message);

private:
    void logToFile(const char *category, const QMessageLogContext &context, const QString &message);

    static LogManager *s_instance;
    QFile *m_file;
    int m_logLevel;
};

class LogManagerGuard
{
public:
    LogManagerGuard(const QString &storagePath);
    LogManagerGuard(const LogManagerGuard&) = delete;
    ~LogManagerGuard();

    LogManagerGuard& operator=(const LogManagerGuard&) = delete;

private:
    LogManager m_manager;
};

#endif // LOGMANAGER_H
