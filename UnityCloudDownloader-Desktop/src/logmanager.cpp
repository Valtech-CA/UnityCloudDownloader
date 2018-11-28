#include "logmanager.h"

#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QBuffer>
#include <QTextStream>

#include <iostream>

LogManager* LogManager::s_instance = nullptr;

thread_local static QByteArray s_messageBuffer;

#ifdef Q_OS_WIN
#define ENDMSG "\x0D\x0A\x0D\x0A"
#else
#define ENDMSG "\n\n"
#endif

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    const auto manager = LogManager::instance();
    if (manager != nullptr)
        manager->logMessage(type, context, message);
}

LogManager::LogManager(const QString &storagePath, QObject *parent)
    : QObject(parent)
    , m_file(nullptr)
    , m_logLevel(4)
{
    auto storageDir = QDir(storagePath);
    storageDir.mkpath(storagePath);
    auto fileName = QStringLiteral("%1.log").arg(QDateTime::currentDateTime().toString(QStringLiteral("yy-MM-dd HH.mm.ss")));
    auto filePath = storageDir.filePath(fileName);
    m_file = new QFile(filePath, this);
    m_file->open(QIODevice::WriteOnly);

    auto allLogs = storageDir.entryInfoList(QStringList{QStringLiteral("*.log")}, QDir::Files, QDir::Name);
    while (allLogs.size() > 10)
    {
        auto oldLog = allLogs.takeFirst();
        QFile::remove(oldLog.absoluteFilePath());
    }
}

LogManager::~LogManager()
{}

void LogManager::logMessage(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    switch (type)
    {
    case QtDebugMsg:
        if (m_logLevel >= 4)
        {
            logToFile("Debug", context, message);
        }
        break;
    case QtInfoMsg:
        if (m_logLevel >= 3)
        {
            logToFile("Info", context, message);
        }
        break;
    case QtWarningMsg:
        if (m_logLevel >= 2)
        {
            logToFile("Warning", context, message);
        }
        break;
    case QtCriticalMsg:
        if (m_logLevel >= 1)
        {
            logToFile("Critical", context, message);
        }
        break;
    case QtFatalMsg:
        if (m_logLevel >= 0)
        {
            logToFile("Fatal", context, message);
        }
        break;
    }
}

void LogManager::logToFile(const char *category, const QMessageLogContext &context, const QString &message)
{
    QBuffer buffer(&s_messageBuffer);
    buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
    {
        QTextStream stream(&buffer);
        stream << category << " (" << context.file << ":" << context.line;
        if (context.function != nullptr && context.function[0] == '\0')
            stream << ", " << context.function;
        stream << "): " << message;
        stream.flush();
        std::cerr << s_messageBuffer.data() << std::endl;
        stream << ENDMSG;
    }
    m_file->write(s_messageBuffer.data(), buffer.size());
    m_file->flush();
}

LogManagerGuard::LogManagerGuard(const QString &storagePath)
    : m_manager(storagePath)
{
    LogManager::s_instance = &m_manager;
    qInstallMessageHandler(&messageHandler);
}

LogManagerGuard::~LogManagerGuard()
{
    qInstallMessageHandler(nullptr);
    if (LogManager::s_instance == &m_manager)
    {
        LogManager::s_instance = nullptr;
    }
}

#undef ENDMSG
