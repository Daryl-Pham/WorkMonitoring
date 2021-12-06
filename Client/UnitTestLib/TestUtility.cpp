#include "stdafx.h"

#include "gtest/gtest.h"
#include "RegistryHelper.h"
#include "Utility.h"

TEST(testUtility_NotRunOnCI, GetDefaultDataDirectory)
{
    // This test is success when BizTelework is installed.
    EXPECT_EQ(std::wstring(L"C:\\ProgramData\\OPTiM\\BizTelework"), Utility::GetDefaultDataDirectory().wstring());
}
