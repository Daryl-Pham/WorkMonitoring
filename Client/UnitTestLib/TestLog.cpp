#include "stdafx.h"

#include <gtest/gtest.h>
#include <atlstr.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "Log.h"

namespace
{

    std::wstring GetContent(std::wstring path)
    {
        std::wifstream file(path);
        EXPECT_TRUE(file.is_open());
        std::wstringstream buf;
        buf << file.rdbuf();

        file.close();

        return buf.str();
    }

}


//Test function LogFile::WriteLog
TEST(testLog, WriteLog)
{
    wchar_t tmp[MAX_PATH] = { 0 };
    ASSERT_NE(0, GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0])));
    auto dst_dir = boost::filesystem::path(tmp).parent_path();
    dst_dir /= L"文字㌠folder";

    // Setup
    if (boost::filesystem::exists(dst_dir))
    {
        boost::filesystem::remove_all(dst_dir);
    }

    LoggerInitialize(LOGGER_LOG_TYPE_INFO, dst_dir.wstring().c_str());

    const char LOGGER_FILE_NAME[] = "TAG"; // For LOG_* macros.

    LOG_ERROR("ERROR");
    LOG_WARN("WARN");
    LOG_INFO("INFO");
    LOG_DEBUG("DEBUG");
    LOG_VERBOSE("VERBOSE");

    LoggerFinalize();

    SYSTEMTIME st;
    GetLocalTime(&st);
    boost::filesystem::path log_path = dst_dir / (boost::format("%04d%02d%02d.txt") % st.wYear% st.wMonth% st.wDay).str();
    EXPECT_TRUE(boost::filesystem::exists(log_path));
    std::wstring log_content = GetContent(log_path.wstring());
    EXPECT_NE(log_content.find(L"TAG(46) ERROR"), std::wstring::npos);
    EXPECT_NE(log_content.find(L"TAG(47) WARN"), std::wstring::npos);
    EXPECT_NE(log_content.find(L"TAG(48) INFO"), std::wstring::npos);
    EXPECT_EQ(log_content.find(L"TAG(49) DEBUG"), std::wstring::npos);
    EXPECT_EQ(log_content.find(L"TAG(50) VERBOSE"), std::wstring::npos);

    // Cleanup directory.
    if (boost::filesystem::exists(dst_dir))
    {
        boost::filesystem::remove_all(dst_dir);
    }
}