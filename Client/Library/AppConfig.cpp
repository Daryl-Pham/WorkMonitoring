#include "stdafx.h"
#include "AppConfig.h"
#include "Environment.h"
#include "Log.h"
#include "RegistryHelper.h"
#include <cpprest/asyncrt_utils.h>

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    // BizTelework in registry
    const wchar_t OptimalBizTelework32Reg[] = _T("SOFTWARE\\OPTiM\\OptimalBizTelework");
    const wchar_t OptimalBizTelework64Reg[] = _T("SOFTWARE\\WOW6432Node\\OPTiM\\OptimalBizTelework");

    const wchar_t API_URL[] = _T("ApiUrl");
    const wchar_t DATA_DIR[] = _T("DataDir");
    const wchar_t VERSION[] = _T("Version");
    const wchar_t UPDATE_URL[] = _T("UpdateUrl");
    const wchar_t TIMER_GET_ACTIVE_APP[] = _T("TimerIntervalGetActiveApplication");
    const wchar_t TIMER_INTERVAL_CHECK_AUTHEN[] = _T("TimerIntervalCheckAuthen");
    const wchar_t UPGRADE_CODE[] = _T("UpgradeCode");
    const wchar_t  SENTRY_DSN[] = _T("SentryDSN");
    const wchar_t  RELEASE_ENVIRONMENT[] = _T("ReleaseEnvironment");
    const wchar_t  BIZ_TELEWORK_UPDATER_RPC_ENDPOINT[] = _T("BizTeleworkUpdaterRPCEndpoint");
    const wchar_t EXECUTED_BIZ_TELEWORK[] = _T("ExecutedBizTelework");
    const wchar_t TIME_DISPLAY_HEALTH_STATUS_REMIND[] = _T("TimeDisplayHealthStatusRemind");
    const wchar_t TIMER_INTERVAL_TO_REMIND_ON_DUTY_DIALOG[] = _T("TimerIntervalToRemidOnDutyDialog");
    const wchar_t TIME_TO_SHOW_ON_DUSTY_DIALOG[] = _T("TimeToShowOnDutyDialog");
    const wchar_t TIME_TO_SHOW_HEALTH_STATUS_DIALOG[] = _T("TimeToShowHealthStatusDialog");
    const wchar_t COPIED_AUTHENTICATION[] = _T("CopiedAuthentication");

    bool GetStringFromRegistry(std::wstring *dst, const wchar_t *valueName)
    {
        if (!dst)
        {
            LOG_WARN("The dst must not be null.");
            return false;
        }

        const CString subKey = Environment::GetInstance()->IsWindowsOS64() ? OptimalBizTelework64Reg : OptimalBizTelework32Reg;

        RegistryHelper regHelper;
        std::wstring value = regHelper.GetRegistryStringValue(subKey, valueName);

        if (value.empty())
        {
            LOG_WARN(" regHelper.GetRegistryStringValue() failed.");
            // To developer: You need install this app from installer. Because the installer write the register which this method requires.
            return false;
        }

        *dst = value;
        return true;
    }

    bool GetDWORDFromRegistry(DWORD* dst, const wchar_t* valueName, HKEY hKey = HKEY_LOCAL_MACHINE)
    {
        if (!dst)
        {
            LOG_WARN("The dst must not be null.");
            return false;
        }

        const CString subKey = Environment::GetInstance()->IsWindowsOS64() ? OptimalBizTelework64Reg : OptimalBizTelework32Reg;
        RegistryHelper regHelper(hKey);
        DWORD value = 0;
        if (!regHelper.GetRegistryDWORDValue(&value, subKey, valueName))
        {
            LOG_WARN("regHelper.GetRegistryDWORDValue() failed.");
            return false;
        }

        *dst = value;
        return true;
    }

    bool SetDWORDToRegistry(DWORD value, const wchar_t* valueName, HKEY hKey = HKEY_LOCAL_MACHINE)
    {
        const CString subKey = Environment::GetInstance()->IsWindowsOS64() ? OptimalBizTelework64Reg : OptimalBizTelework32Reg;
        RegistryHelper regHelper(hKey);

        if (!regHelper.SetRegistryDWORDValue(subKey.GetString(), valueName, value))
        {
            LOG_WARN("regHelper.SetRegistryDWORDValue() failed.");
            return false;
        }

        return true;
    }

}  // namespace

bool AppConfig::GetApiUrl(std::wstring *dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    return GetStringFromRegistry(dst, API_URL);
}

bool AppConfig::GetDataDir(std::wstring* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    std::wstring userProfile = L"";
    wchar_t* buf = nullptr;
    size_t sz = 0;
    errno_t e;
    if ((e = _wdupenv_s(&buf, &sz, L"USERPROFILE")) == 0 && buf != nullptr)
    {
        userProfile.append(buf).append(L"\\");
        free(buf);
    }
    else
    {
        LOG_WARN("_wdupenv_s() failed. %d", e);
        return false;
    }

    if (GetStringFromRegistry(dst, DATA_DIR))
    {
        *dst = userProfile.append(*dst);
        return true;
    }

    return false;
}

bool AppConfig::GetUpdateUrl(std::wstring* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    return GetStringFromRegistry(dst, UPDATE_URL);
}

bool AppConfig::GetUpgradeCode(std::wstring* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    return GetStringFromRegistry(dst, UPGRADE_CODE);
}

bool AppConfig::GetVersion(std::wstring* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    return GetStringFromRegistry(dst, VERSION);
}

bool AppConfig::GetTimerIntervalGetActiveApplication(DWORD* timer)
{
    if (!timer)
    {
        LOG_WARN("The timer must not be null.");
        return false;
    }

    return GetDWORDFromRegistry(timer, TIMER_GET_ACTIVE_APP);
}

bool AppConfig::GetTimerIntervalCheckAuthen(DWORD* timer)
{
    if (!timer)
    {
        LOG_WARN("The timer must not be null.");
        return false;
    }

    return GetDWORDFromRegistry(timer, TIMER_INTERVAL_CHECK_AUTHEN);
}

bool AppConfig::GetSentryDSN(std::string* dsn)
{
    if (!dsn)
    {
        LOG_WARN("The dsn must not be null.");
        return false;
    }

    const CString subKey = Environment::GetInstance()->IsWindowsOS64() ? OptimalBizTelework64Reg : OptimalBizTelework32Reg;

    RegistryHelper regHelper;
    std::wstring sentryDSN = regHelper.GetRegistryStringValue(subKey, SENTRY_DSN);

    if (sentryDSN.empty())
    {
        LOG_WARN(" regHelper.GetRegistryStringValue() failed.");
        // To developer: You need install this app from installer. Because the installer write the register which this method requires.
        return false;
    }
    *dsn = utility::conversions::to_utf8string(sentryDSN);

    return true;
}

bool AppConfig::GetReleaseEnvironment(std::string* environment)
{
    if (!environment)
    {
        LOG_WARN("The environment must not be null.");
        return false;
    }

    std::wstring wEnvironment;
    if (!GetStringFromRegistry(&wEnvironment, RELEASE_ENVIRONMENT) || (wEnvironment == L"Development"))
    {
        return false;
    }

    *environment = utility::conversions::to_utf8string(wEnvironment);
    return true;
}

bool AppConfig::GetBizTeleworkUpdaterRPCEndpoint(std::wstring* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }
    return GetStringFromRegistry(dst, BIZ_TELEWORK_UPDATER_RPC_ENDPOINT);
}

bool AppConfig::GetExecutedBizTelework(bool* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, EXECUTED_BIZ_TELEWORK))
    {
        return false;
    }

    *dst = (v != 0);
    return true;
}

bool AppConfig::SetExecutedBizTelework(bool value)
{
    DWORD dValue = value ? 1 : 0;
    return SetDWORDToRegistry(dValue, EXECUTED_BIZ_TELEWORK);
}

bool AppConfig::GetTimeDisplayHealthStatusRemind(int64_t* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, TIME_DISPLAY_HEALTH_STATUS_REMIND, HKEY_CURRENT_USER))
    {
        return false;
    }

    *dst = v;
    return true;
}

bool AppConfig::SetTimeDisplayHealthStatusRemind(const int64_t time)
{
    return SetDWORDToRegistry(time, TIME_DISPLAY_HEALTH_STATUS_REMIND, HKEY_CURRENT_USER);
}

bool AppConfig::DeleteTimeDisplayHealthStatusRemind()
{
    RegistryHelper regTimeDisplayHealthStatusRemind;
    const CString subKey = Environment::GetInstance()->IsWindowsOS64() ? Telework64Reg : Telework32Reg;
    if (!regTimeDisplayHealthStatusRemind.DeleteRegistryValue(subKey.GetString(), TIME_DISPLAY_HEALTH_STATUS_REMIND))
    {
        LOG_WARN("AppConfig::DeleteTimeDisplayHealthStatusRemind() TIME_DISPLAY_HEALTH_STATUS_REMIND failed.");
        return false;
    }
    return true;
}

bool AppConfig::GetTimerIntervalToRemidOnDutyDialog(int64_t* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, TIMER_INTERVAL_TO_REMIND_ON_DUTY_DIALOG))
    {
        return false;
    }

    *dst = v;
    return true;
}

bool AppConfig::GetTimeToShowOnDutyDialog(int64_t* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, TIME_TO_SHOW_ON_DUSTY_DIALOG))
    {
        return false;
    }

    *dst = v;
    return true;
}

bool AppConfig::GetTimeToShowHealthStatusDialog(int64_t* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, TIME_TO_SHOW_HEALTH_STATUS_DIALOG))
    {
        return false;
    }

    *dst = v;
    return true;
}

bool AppConfig::GetCopiedAuthentication(bool* dst)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    DWORD v = 0;
    if (!GetDWORDFromRegistry(&v, COPIED_AUTHENTICATION))
    {
        return false;
    }

    *dst = (v != 0);
    return true;
}

bool AppConfig::SetCopiedAuthentication(bool value)
{
    DWORD dValue = value ? 1 : 0;
    return SetDWORDToRegistry(dValue, COPIED_AUTHENTICATION, HKEY_CURRENT_USER);
}
