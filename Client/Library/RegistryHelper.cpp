#include "stdafx.h"
#include "Environment.h"
#include "RegistryHelper.h"
#include "Utility.h"
#include "Log.h"
#include <system_error>

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t TIME_DISPLAY_HEALTH_STATUS_REMIND[] = _T("TimeDisplayHealthStatusRemind");
    const wchar_t COPIED_AUTHENTICATION[] = _T("CopiedAuthentication");
}

static const std::string ExceptionOpenRegistry = "Error open registry to read";
static const std::string ExceptionQueryStringFromRegistry = "Error query string value from registry";
static const std::string ExceptionQueryBinaryFromRegistry = "Error query binary value from registry";

RegistryHelper::RegistryHelper(HKEY rootKey) noexcept
    : m_RegistryKey(rootKey)
{
}

LSTATUS RegistryHelper::OpenRegistry(CString subKey, REGSAM regsam)
{

    LSTATUS retCode = ERROR_INVALID_FUNCTION;

    if (Environment::GetInstance()->IsWindowsOS64())
    {
        retCode = m_RegistryKey.Open(m_RegistryKey.m_hKey, subKey, regsam | KEY_WOW64_64KEY);
    }
    else
    {
        retCode = m_RegistryKey.Open(m_RegistryKey.m_hKey, subKey, regsam | KEY_WOW64_32KEY);
    }

    return retCode;
}

LSTATUS RegistryHelper::OpenRegistryToRead(CString subKey)
{
    return OpenRegistry(subKey, KEY_READ);
}

std::wstring RegistryHelper::GetRegistryStringValue(CString subKey, CString valueName)
{
    std::wstring result = _T("");
    if (m_RegistryKey.m_hKey == HKEY_LOCAL_MACHINE)
    {
        RegistryHelper regHelper(HKEY_CURRENT_USER);
        result = regHelper.GetRegistryStringValue(subKey, valueName);
        if (!result.empty())
        {
            return result;
        }
    }

    TCHAR qval[ValueLength] = {};
    ULONG qlen = ValueLength - 1;

    LSTATUS retCode = ERROR_INVALID_FUNCTION;

    if ((retCode = OpenRegistryToRead(subKey)) == ERROR_SUCCESS)
    {
        if ((retCode = m_RegistryKey.QueryStringValue(valueName, qval, &qlen)) != ERROR_SUCCESS && retCode != ERROR_FILE_NOT_FOUND)
        {
            LOG_WARN("QueryStringValue() failed. Retcode %ld: %s.", retCode, std::system_category().message(retCode).c_str());
        }
        else
        {
            result = qval;
        }
    }
    else
    {
        LOG_WARN("OpenRegistryToRead() failed.");
    }

    m_RegistryKey.Close();

    return result;
}

std::vector<BYTE> RegistryHelper::GetRegistryBinaryValue(CString subKey, CString valueName)
{
    std::vector<BYTE>   result(0);
    ULONG               size;
    LSTATUS retCode = ERROR_INVALID_FUNCTION;

    if (m_RegistryKey.m_hKey == HKEY_LOCAL_MACHINE)
    {
        RegistryHelper regHelper(HKEY_CURRENT_USER);
        result = regHelper.GetRegistryBinaryValue(subKey, valueName);
        if (!result.empty())
        {
            return result;
        }
    }

    if ((retCode = OpenRegistryToRead(subKey)) == ERROR_SUCCESS)
    {
        if ((retCode = m_RegistryKey.QueryBinaryValue(valueName, NULL, &size)) == ERROR_SUCCESS)
        {
            result.resize(size);
            LPBYTE  pResult = &result[0];
            if (m_RegistryKey.QueryBinaryValue(valueName, pResult, &size) != ERROR_SUCCESS)
            {
                //TODO write log
            }
        }
        else
        {
            //TODO write log
        }
    }
    else
    {
        //TODO write log
    }


    m_RegistryKey.Close();

    return result;
}

bool RegistryHelper::GetRegistryDWORDValue(DWORD *dst, CString subKey, CString valueName)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    if (m_RegistryKey.m_hKey == HKEY_LOCAL_MACHINE && IsOpenCurrentUser(valueName))
    {
        RegistryHelper regHelper(HKEY_CURRENT_USER);
        if (regHelper.GetRegistryDWORDValue(dst, subKey, valueName))
        {
            return true;
        }
    }

    DWORD result = 0;
    LSTATUS retCode = ERROR_INVALID_FUNCTION;

    if ((retCode = OpenRegistryToRead(subKey)) == ERROR_SUCCESS)
    {
        if ((retCode = m_RegistryKey.QueryDWORDValue(valueName, result)) != ERROR_SUCCESS)
        {
            LOG_WARN("QueryDWORDValue() failed. Retcode %ld: %s.", retCode, std::system_category().message(retCode).c_str());
            m_RegistryKey.Close();
            return false;
        }
    }
    else
    {
        LOG_WARN("OpenRegistryToRead() failed. Retcode %ld: %s.", retCode, std::system_category().message(retCode).c_str());
        return false;
    }

    m_RegistryKey.Close();

    *dst = result;
    return true;
}

LSTATUS RegistryHelper::OpenRegistryToWrite(const std::wstring& subKey)
{
    return OpenRegistry(CString(subKey.c_str()), KEY_WRITE);
}

bool RegistryHelper::SetRegistryStringValue(const std::wstring& subKey, const std::wstring& valueName, const std::wstring& value)
{
    if (!OpenRegistryToWrite(subKey))
    {
        if (m_RegistryKey.SetStringValue(valueName.c_str(), value.c_str(), REG_SZ) != ERROR_SUCCESS)
        {
            LOG_WARN("SetStringValue() failed.");
            return false;
        }
        return true;
    }

    return false;
}

bool RegistryHelper::SetRegistryBinaryValue(const std::wstring& subKey, const std::wstring& valueName, const std::vector<BYTE>& value)
{
    if (!OpenRegistryToWrite(subKey))
    {
        const DWORD dataSize = static_cast<DWORD>(value.size() * sizeof(BYTE));
        if (m_RegistryKey.SetBinaryValue(valueName.c_str(), (LPBYTE)(&value[0]), dataSize) != ERROR_SUCCESS)
        {
            LOG_WARN("SetBinaryValue() failed.");
            return false;
        }
        return true;
    }

    return false;
}

bool RegistryHelper::SetRegistryDWORDValue(const std::wstring& subKey, const std::wstring& valueName, DWORD value)
{
    LSTATUS r;

    if ((r = OpenRegistryToWrite(subKey)) != ERROR_SUCCESS)
    {
        LOG_WARN("OpenRegistryToWrite() failed. %ld", r);
        return false;
    }
    if ((r = m_RegistryKey.SetDWORDValue(valueName.c_str(), value)) != ERROR_SUCCESS)
    {
        LOG_WARN("m_RegistryKey.SetDWORDValue() failed. %ld", r);
        return false;
    }

    return true;
}

bool RegistryHelper::DeleteRegistryValue(const std::wstring& subKey, const std::wstring& valueName)
{
    LSTATUS r;

    if ((r = OpenRegistryToWrite(subKey)) != ERROR_SUCCESS)
    {
        LOG_WARN("OpenRegistryToWrite() failed. %ld", r);
        return false;
    }

    if ((r = m_RegistryKey.DeleteValue(valueName.c_str())) != ERROR_SUCCESS)
    {
        LOG_WARN("m_RegistryKey.DeleteValue() failed. %ld", r);
        return false;
    }

    return true;
}

bool RegistryHelper::IsOpenCurrentUser(CString valueName)
{
    if (valueName == TIME_DISPLAY_HEALTH_STATUS_REMIND || valueName == COPIED_AUTHENTICATION)
    {
        return true;
    }
    return false;
}
