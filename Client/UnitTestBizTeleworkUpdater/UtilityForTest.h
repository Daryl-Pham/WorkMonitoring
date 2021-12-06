#pragma once

#include <string>
#include <boost/filesystem.hpp>

class UtilityForTest
{
public:
    static boost::filesystem::path GetSelfPath();

    static boost::filesystem::path GetMsiForTestingPath();

    // This upgrade code is same as MsiForTesting\TestParameter.wxi.
    static const std::wstring EXPECTED_UPGRAD_CODE;
    static const std::wstring VERSION;
};
