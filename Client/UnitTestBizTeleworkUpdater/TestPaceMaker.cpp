#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <chrono>
#include <Windows.h>
#include <PaceMaker.h>

namespace
{
    boost::filesystem::path GetDestinationDirectory()
    {
        wchar_t tmp[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0]));
        auto dstDir = boost::filesystem::path(tmp).parent_path() / L"test_pace_maker";

        return dstDir;
    }

}  // namespace

TEST(testPaceMaker, Wait)
{
    const unsigned int begin = 2;
    const unsigned int end = 6;
    const unsigned int interval = 4;

    auto dstDir = GetDestinationDirectory();
    auto updateDatetimePath = dstDir / L"/update_datetime.txt";

    // Remove destination directory.
    boost::filesystem::remove_all(dstDir);

    PaceMaker pm(std::make_unique<PaceMaker::Config>(begin, end, interval, dstDir.wstring()));

    HANDLE stopEventHandler = CreateEvent(NULL, TRUE, FALSE, NULL);
    auto beginTime = std::chrono::system_clock::now();
    EXPECT_EQ(PaceMaker::FINISHED_TO_SLEEP, pm.Wait(stopEventHandler));
    auto duration = std::chrono::system_clock::now() - beginTime;

    const unsigned int offsetForTest = 1; // in second
    EXPECT_GE(std::chrono::duration_cast<std::chrono::seconds>(duration).count(), begin - offsetForTest);
    EXPECT_FALSE(boost::filesystem::exists(updateDatetimePath));

    // Cleanup resources.
    CloseHandle(stopEventHandler);
    boost::filesystem::remove_all(dstDir);
}

TEST(testPaceMaker, StopWaitingByTheStopEventHandler)
{
    const unsigned int begin = 10;
    const unsigned int end = 30;
    const unsigned int interval = 5;

    auto dstDir = GetDestinationDirectory();
    auto updateDatetimePath = dstDir / L"/update_datetime.txt";

    // Remove destination directory.
    boost::filesystem::remove_all(dstDir);

    PaceMaker pm(std::make_unique<PaceMaker::Config>(begin, end, interval, dstDir.wstring()));

    HANDLE stopEventHandler = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetEvent(stopEventHandler);

    auto beginTime = std::chrono::system_clock::now();
    EXPECT_EQ(PaceMaker::GOT_EVENT, pm.Wait(stopEventHandler));
    auto duration = std::chrono::system_clock::now() - beginTime;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(duration).count(), interval);
    EXPECT_TRUE(boost::filesystem::exists(updateDatetimePath));

    // Cleanup resources.
    CloseHandle(stopEventHandler);
    boost::filesystem::remove_all(dstDir);
}

TEST(testPaceMaker, WaitNotToSleep)
{
    const unsigned int begin = 100;
    const unsigned int end = 200;
    const unsigned int interval = 10;

    auto dstDir = GetDestinationDirectory();
    auto updateDatetimePath = dstDir / L"/update_datetime.txt";

    // Reset destination directory.
    boost::filesystem::create_directories(dstDir);

    // Make condition of update_datetime.txt already exists.
    int64_t currentSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    int64_t someTimesAgo = currentSecond - 2;
    std::wofstream dstFile(updateDatetimePath.wstring(), std::ios::out | std::ios::trunc);
    dstFile << someTimesAgo;
    dstFile.close();

    PaceMaker pm(std::make_unique<PaceMaker::Config>(begin, end, interval, dstDir.wstring()));

    HANDLE stopEventHandler = CreateEvent(NULL, TRUE, FALSE, NULL);
    auto beginTime = std::chrono::system_clock::now();
    EXPECT_EQ(PaceMaker::FINISHED_TO_SLEEP, pm.Wait(stopEventHandler));
    auto duration = std::chrono::system_clock::now() - beginTime;

    // This conditon ensures that it does not expire in less than 1 second.
    auto durationSecond = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    EXPECT_LT(durationSecond, 1);

    EXPECT_FALSE(boost::filesystem::exists(updateDatetimePath));

    // Cleanup resources.
    CloseHandle(stopEventHandler);
    boost::filesystem::remove_all(dstDir);
}

TEST(testPaceMaker, WaitDistantFuture)
{
    const unsigned int begin = 1;
    const unsigned int end = 2;
    const unsigned int interval = 1;

    auto dstDir = GetDestinationDirectory();
    auto updateDatetimePath = dstDir / L"/update_datetime.txt";

    // Reset destination directory.
    boost::filesystem::create_directories(dstDir);

    // Make condition of update_datetime.txt already exists.
    // update_datetime.txt must be recreated on calling PaceMaker::Wait().
    int64_t currentSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    int64_t distantFuture = currentSecond + (end * 100);
    std::wofstream dstFile(updateDatetimePath.wstring(), std::ios::out | std::ios::trunc);
    dstFile << distantFuture;
    dstFile.close();

    PaceMaker pm(std::make_unique<PaceMaker::Config>(begin, end, interval, dstDir.wstring()));

    HANDLE stopEventHandler = CreateEvent(NULL, TRUE, FALSE, NULL);
    auto beginTime = std::chrono::system_clock::now();
    EXPECT_EQ(PaceMaker::FINISHED_TO_SLEEP, pm.Wait(stopEventHandler));
    auto duration = std::chrono::system_clock::now() - beginTime;

    EXPECT_GE(std::chrono::duration_cast<std::chrono::seconds>(duration).count(), interval * 2);
    EXPECT_FALSE(boost::filesystem::exists(updateDatetimePath));

    // Cleanup resources.
    CloseHandle(stopEventHandler);
    boost::filesystem::remove_all(dstDir);
}

TEST(testPaceMaker, UpdateDatetimeIsUnexpectedValue)
{
    const unsigned int begin = 1;
    const unsigned int end = 2;
    const unsigned int interval = 1;

    auto dstDir = GetDestinationDirectory();
    auto updateDatetimePath = dstDir / L"/update_datetime.txt";

    // Reset destination directory.
    boost::filesystem::create_directories(dstDir);

    // Make condition of update_datetime.txt has unexpected value.
    std::wofstream dstFile(updateDatetimePath.wstring(), std::ios::out | std::ios::trunc);
    dstFile << "This is\nUnexpected value\n";
    dstFile.close();

    PaceMaker pm(std::make_unique<PaceMaker::Config>(begin, end, interval, dstDir.wstring()));

    HANDLE stopEventHandler = CreateEvent(NULL, TRUE, FALSE, NULL);
    auto beginTime = std::chrono::system_clock::now();
    EXPECT_EQ(PaceMaker::FINISHED_TO_SLEEP, pm.Wait(stopEventHandler));
    auto duration = std::chrono::system_clock::now() - beginTime;

    EXPECT_GE(std::chrono::duration_cast<std::chrono::seconds>(duration).count(), interval * 2);
    EXPECT_FALSE(boost::filesystem::exists(updateDatetimePath));

    // Cleanup resources.
    CloseHandle(stopEventHandler);
    boost::filesystem::remove_all(dstDir);
}