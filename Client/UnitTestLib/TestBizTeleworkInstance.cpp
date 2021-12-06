#include "stdafx.h"
#include "Common.h"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>
#include <boost/compute/detail/getenv.hpp>
#include "BizTeleworkInstance.h"

namespace bfs = boost::filesystem;
const wchar_t* MutexName = L"{BA49C45E-B29A-4359-A07C-51B65B5571AD}";

//Case: The mutext is created. Expect BizTeleworkInstance::CheckAlready() return TRUE.
TEST(testBizTeleworkInstance, MutexIsCreated)
{
    HANDLE hMutex = ::CreateMutex(NULL, TRUE, MutexName);
    BizTeleworkInstance instance;
    EXPECT_TRUE(instance.CheckAlready());
    if (hMutex)
    {
        ::ReleaseMutex(hMutex);
        ::CloseHandle(hMutex);
    }
}

//Case: The mutext isn't created. Expect BizTeleworkInstance::CheckAlready() return FALSE.
TEST(testBizTeleworkInstance, MutexIsNOTCreated)
{
    BizTeleworkInstance instance;
    EXPECT_FALSE(instance.CheckAlready());
}