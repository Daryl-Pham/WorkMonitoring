#include <Windows.h>
#include "UtilityForTest.h"


const std::wstring UtilityForTest::EXPECTED_UPGRAD_CODE = L"{ED6E254E-1C15-4480-8AFE-24C958C2BB2E}";
const std::wstring UtilityForTest::VERSION = L"1.0.0.0";

boost::filesystem::path UtilityForTest::GetSelfPath()
{
    wchar_t tmp[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0]));
    return boost::filesystem::path(tmp);
}

boost::filesystem::path UtilityForTest::GetMsiForTestingPath()
{
    return GetSelfPath().parent_path() / L"MsiForTesting.msi";
}
