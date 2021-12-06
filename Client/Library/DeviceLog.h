#pragma once

#include "Utility.h"
#include "Common.h"
#include "Logging.h"
#include "Log.h"

class DeviceLog
{
public:
    bool ReadDeviceLog(LogLines* logLines, bool* keyboardLogFileExists, bool* mouseLogFileExists);
    bool DeleteDeviceLog(const bool keyboardLogFileExists, const bool mouseLogFileExists);

    bool ReadFullDeviceLog(LogLines* logLines, bool* keyboardLogFileExists, bool* mouseLogFileExists);

    void SendLog(const LogLines logLines,
                 const bool keyboardLogFileExists,
                 const bool mouseLogFileExists);

    bool ReadMouseLog(LogLines* logLines);
    bool ReadKeyboardLog(LogLines* logLines);

private:
    Logging m_KeyboardLogFile = Logging(Utility::GetDefaultDataDirectory(), KeyboardLogFileName);
    Logging m_MouseLogFile = Logging(Utility::GetDefaultDataDirectory(), MouseLogFileName);
};
