#include "stdafx.h"
#include "Log.h"

#include <stdarg.h>
#include <sstream>
#include <Windows.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/compute/detail/getenv.hpp>
#include <mutex>
#include "Utility.h"

#define SENTRY_BUILD_STATIC 1
#include "sentry.h"

#include "AppConfig.h"

namespace bfs = boost::filesystem;

#define LOG_LINE_BUFFER_SIZE (10 * 1024)

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const LoggerLogType DEFAULT_LOGGER_TYPE = LOGGER_LOG_TYPE_ERROR;

    LoggerLogType log_type = DEFAULT_LOGGER_TYPE;

    char tmp_buf[LOG_LINE_BUFFER_SIZE] = { 0 };

    char log_buf[LOG_LINE_BUFFER_SIZE] = { 0 };

    boost::filesystem::path log_directory;

    bool isInitSentry = false;

    const char* directoryLogSentry = ".sentry_native";

    bfs::path databaseSentryPath = Utility::GetDefaultDataDirectory() / bfs::path(directoryLogSentry);

bool initSentry()
{
    sentry_options_t* options = sentry_options_new();
    std::string environment;
    AppConfig appConfig;
    if (!appConfig.GetReleaseEnvironment(&environment))
    {
        return false;
    }
    sentry_options_set_environment(options, environment.c_str());

    std::string dsn;
    if (!appConfig.GetSentryDSN(&dsn))
    {
        return false;
    }
    sentry_options_set_dsn(options, dsn.c_str());

    if (!databaseSentryPath.empty())
    {
        sentry_options_set_database_path(options, databaseSentryPath.string().c_str());
    }

    return (sentry_init(options) == 0);
}

char *logTypeToString(LoggerLogType type)
{
    switch (type)
    {
    case LOGGER_LOG_TYPE_ERROR:
        return "Error   ";
    case LOGGER_LOG_TYPE_WARN:
        return "Warn    ";
    case LOGGER_LOG_TYPE_INFO:
        return "Info    ";
    case LOGGER_LOG_TYPE_DEBUG:
        return "Debug   ";
    case LOGGER_LOG_TYPE_VERBOSE:
        return "Verbose ";
    default:
        return "Undef   ";
    }
}

bool loggerShouldNotOutput(LoggerLogType type)
{
    // log_type で出力すべきログを制限している
    return type > log_type;
}

void loggerLogInternal(FILE *log_file, const SYSTEMTIME& st, LoggerLogType type, int line,
                       const char* tag, _Printf_format_string_ char const* const format, va_list params)
{
    if (loggerShouldNotOutput(type))
    {
        return;
    }

    static std::mutex mutex;
    std::lock_guard<std::mutex> lck(mutex);
    ZeroMemory(tmp_buf, _countof(tmp_buf));
    ZeroMemory(log_buf, _countof(log_buf));

    char datetime[128] = { 0 };
    if (vsprintf_s(tmp_buf, _countof(tmp_buf), format, params) < 0)
    {
        OutputDebugStringA("sprintf_s(tmp_buf) failed.");
        return;
    }
    else if (sprintf_s(datetime, _countof(datetime), "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond) < 0)
    {
        OutputDebugStringA("sprintf_s(datetime) failed.");
        return;
    }
    else if (sprintf_s(log_buf, _countof(log_buf), "%s %s%s(%d) %s\n", datetime, logTypeToString(type), tag, line, tmp_buf) < 0)
    {
        OutputDebugStringA("sprintf_s(log_buf) failed.");
        return;
    }

    OutputDebugStringA(log_buf);

    if (log_file)
    {
        fprintf(log_file, "%s", log_buf);
        fflush(log_file);
    }
}

FILE* loggerGetFilePointer(const SYSTEMTIME& st)
{
    try
    {
        FILE* log_file = nullptr;

        if (!log_directory.empty())
        {
            if ((!boost::filesystem::exists(log_directory)) && (!boost::filesystem::create_directories(log_directory)))
            {
                loggerLogInternal(nullptr, st, LOGGER_LOG_TYPE_ERROR,
                    __LINE__, LOGGER_FILE_NAME, "boost::filesystem::create_directories() failed.", va_list());
                return nullptr;
            }

            boost::filesystem::path log_path(log_directory);
            char log_name[_MAX_PATH + 1] = { 0 };
            sprintf_s(log_name, "%d%02d%02d.txt", st.wYear, st.wMonth, st.wDay);
            log_path /= log_name;

            log_file = _wfsopen(log_path.wstring().c_str(), L"a+", _SH_DENYNO);
            if (!log_file)
            {
                loggerLogInternal(nullptr, st, LOGGER_LOG_TYPE_ERROR, __LINE__, LOGGER_FILE_NAME, "fopen_s() failed.", va_list());
                return nullptr;
            }
            else
            {
                loggerLogInternal(log_file, st, LOGGER_LOG_TYPE_DEBUG, __LINE__, LOGGER_FILE_NAME, "Start logging.", va_list());
                return log_file;
            }
        }
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        std::stringstream s;
        s << "Failed by boost::filesystem::filesystem_error " << e.what();
        loggerLogInternal(nullptr, st, LOGGER_LOG_TYPE_ERROR, __LINE__, LOGGER_FILE_NAME, s.str().c_str(), va_list());
        return nullptr;
    }
    catch (const std::exception& e)
    {
        std::stringstream s;
        s << "Failed by std::exception " << e.what();
        loggerLogInternal(nullptr, st, LOGGER_LOG_TYPE_ERROR, __LINE__, LOGGER_FILE_NAME, s.str().c_str(), va_list());
        return nullptr;
    }
    catch (...)
    {
        loggerLogInternal(nullptr, st, LOGGER_LOG_TYPE_ERROR, __LINE__, LOGGER_FILE_NAME, "Failed by an unknown exception", va_list());
        return nullptr;
    }

    return nullptr;
}

}  // namespace

void LoggerInitialize(LoggerLogType type, const wchar_t *directory)
{
    log_type = type;
    log_directory = directory;

    if (!isInitSentry)
    {
        isInitSentry = initSentry();
    }
}

void LoggerFinalize()
{
    log_type = DEFAULT_LOGGER_TYPE;
    log_directory = "";
    sentry_shutdown();
}

void LoggerLog(LoggerLogType type, int line, const char *tag, _Printf_format_string_ char const *const format, ...)
{
    if (loggerShouldNotOutput(type))
    {
        return;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);

    FILE* log_file = loggerGetFilePointer(st);

    va_list params;
    va_start(params, format);

    loggerLogInternal(log_file, st, type, line, tag, format, params);
    // Send error log to sentry server
    if (type == LOGGER_LOG_TYPE_ERROR && isInitSentry)
    {
        sentry_uuid_s uuidSentry = sentry_capture_event(sentry_value_new_message_event(SENTRY_LEVEL_ERROR, logTypeToString(type), log_buf));
    }

    if (log_file)
    {
        fclose(log_file);
        log_file = nullptr;
    }

    va_end(params);
}
