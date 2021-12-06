#include "stdafx.h"
#include "DeviceLog.h"
#include "HttpStatusCode.h"
#include <boost/timer/timer.hpp>
#include "CpprestRequestImpl.h"
#include "LastActivityLog.h"
#include "GettingRequest.h"

namespace
{

    const char* TAG_DEVICELOG = LOGGER_FILE_NAME_MCRO;

}  // namespace

bool DeviceLog::ReadDeviceLog(LogLines* logLines, bool* keyboardLogFileExists, bool* mouseLogFileExists)
{
    if (!m_KeyboardLogFile.ReadLog(*logLines))
    {
        LOG_TAG_INFO(TAG_DEVICELOG, "There is no keyboard operation.");
        *keyboardLogFileExists = false;
    }
    else
    {
        *keyboardLogFileExists = true;
    }

    if (!m_MouseLogFile.ReadLog(*logLines))
    {
        LOG_TAG_INFO(TAG_DEVICELOG, "There is no mouse operation.");
        *mouseLogFileExists = false;
    }
    else
    {
        *mouseLogFileExists = true;
    }

    return true;
}

bool DeviceLog::ReadFullDeviceLog(LogLines* logLines, bool* keyboardLogFileExists, bool* mouseLogFileExists)
{
    BOOL existFile = FALSE;

    if (!m_KeyboardLogFile.ReadLog(logLines, &existFile))
    {
        LOG_TAG_INFO(TAG_DEVICELOG, "m_KeyboardLogFile.ReadLog() failed.");
        *keyboardLogFileExists = false;
        return false;
    }
    else
    {
        *keyboardLogFileExists = existFile;
    }

    if (!m_MouseLogFile.ReadLog(logLines, &existFile))
    {
        LOG_TAG_INFO(TAG_DEVICELOG, "m_MouseLogFile.ReadLog() failed.");
        *mouseLogFileExists = false;
        *keyboardLogFileExists = false;
        return false;
    }
    else
    {
        *mouseLogFileExists = existFile;
    }

    return true;
}

bool DeviceLog::DeleteDeviceLog(const bool keyboardLogFileExists, const bool mouseLogFileExists)
{
    if (keyboardLogFileExists && !m_KeyboardLogFile.DeleteLog())
    {
        LOG_TAG_ERROR(TAG_DEVICELOG, "keyboardLogFile.DeleteLog() failed.");
        return FALSE;
    }
    else if (mouseLogFileExists && !m_MouseLogFile.DeleteLog())
    {
        LOG_TAG_ERROR(TAG_DEVICELOG, "mouseLogFile.DeleteLog() failed.");
        return FALSE;
    }

    return TRUE;
}

void DeviceLog::SendLog(const LogLines logLines,
                        const bool keyboardLogFileExists,
                        const bool mouseLogFileExists)
{
    auto thr = std::thread([logLines, keyboardLogFileExists, mouseLogFileExists]()
    {
        std::wstring jsonStrigify;
        if (!Logging::DeviceLogStringifyAsJsonBody(logLines, &jsonStrigify))
        {
            LOG_TAG_ERROR(TAG_DEVICELOG, "Logging::StringifyAsJsonBody() failed.");
            return;
        }
        boost::timer::cpu_timer timer;

        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        auto callContext = request->CallWindowsDeviceLogs(jsonStrigify);
        if (callContext == nullptr)
        {
            LOG_TAG_ERROR(TAG_DEVICELOG, "request.CallWindowsDeviceLogs() failed.");
            return;
        }
        else if (!callContext->Wait())
        {
            LOG_TAG_ERROR(TAG_DEVICELOG, "callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
            LOG_TAG_ERROR(TAG_DEVICELOG, "callContext->Wait() failed.");
            LOG_TAG_WARN(TAG_DEVICELOG, "Requesting jsonStrigify = %S", jsonStrigify.c_str());
            return;
        }
        else if (callContext->StatusCode() != HTTP_STATUS_CODE_CREATED)
        {
            LOG_TAG_ERROR(TAG_DEVICELOG, "The request is failed. StatusCode: %d", callContext->StatusCode());
            LOG_TAG_WARN(TAG_DEVICELOG, "Requesting jsonStrigify = %S", jsonStrigify.c_str());
            return;
        }
        else
        {
            // "%w" means wall clock time.
            LOG_TAG_INFO(TAG_DEVICELOG, "callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
            LOG_TAG_INFO(TAG_DEVICELOG, "Succeeded to upload an event log.");
            DeviceLog threadDeviceLog;
            threadDeviceLog.DeleteDeviceLog(keyboardLogFileExists, mouseLogFileExists);
        }
    });
    thr.detach();
}

bool DeviceLog::ReadMouseLog(LogLines* logLines)
{
    return m_MouseLogFile.ReadLog(*logLines);
}

bool DeviceLog::ReadKeyboardLog(LogLines* logLines)
{
    return m_KeyboardLogFile.ReadLog(*logLines);
}
