#pragma once

#include <string>
#include <vector>
#include <atlstr.h>
#include "Common.h"

static const DWORD ValueLength = 256;

class RegistryHelper
{

public:
    explicit RegistryHelper(HKEY rootKey = HKEY_LOCAL_MACHINE) noexcept;
    RegistryHelper(const RegistryHelper&) = delete;  // Disable copy constructor.
    RegistryHelper& operator=(const RegistryHelper&) = delete;  // Disable assignment operator.
    virtual ~RegistryHelper() = default;

    std::wstring      GetRegistryStringValue(CString subKey, CString valueName);
    std::vector<BYTE> GetRegistryBinaryValue(CString subKey, CString valueName);
    bool              GetRegistryDWORDValue(DWORD *dst, CString subKey, CString valueName);

    bool              SetRegistryStringValue(const std::wstring& subKey, const std::wstring& valueName, const std::wstring& value);
    bool              SetRegistryBinaryValue(const std::wstring& subKey, const std::wstring& valueName, const std::vector<BYTE>& value);
    bool              SetRegistryDWORDValue(const std::wstring& subKey, const std::wstring& valueName, DWORD value);

    virtual bool      DeleteRegistryValue(const std::wstring& subKey, const std::wstring& valueName);

private:

    CRegKey m_RegistryKey;  // m_RegistryKey.m_hKey will remove on CRegKey's destructor.

    LSTATUS OpenRegistry(CString subKey, REGSAM regsam);
    LSTATUS OpenRegistryToRead(CString subKey);
    LSTATUS OpenRegistryToWrite(const std::wstring& subKey);
    bool IsOpenCurrentUser(CString valueName);
};

