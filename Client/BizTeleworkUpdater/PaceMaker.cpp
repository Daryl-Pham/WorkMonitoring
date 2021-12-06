#include "pch.h"
#include <cassert>
#include <random>
#include <Log.h>
#include <chrono>
#include <inttypes.h>
#include <boost/filesystem.hpp>
#include <Utility.h>
#include <Windows.h>
#include "ConfigUtility.h"
#include "PaceMaker.h"

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const int64_t RANGE_BEGIN_SECOND = 3600;
    const int64_t RANGE_END_SECOND   = 3600 * 24 * 7;
    const int64_t SLEEP_INTERVAL_SECOND = 10;

    const wchar_t UPDATE_DATETIME_TXT[] = L"update_datetime.txt";
}

std::unique_ptr<PaceMaker::Config> PaceMaker::Config::Default()
{
    std::wstring dir = ConfigUtility::GetDataDirectory();
    if (dir.empty())
    {
        LOG_WARN("ConfigUtility::GetDataDirectory() failed.");
        return nullptr;
    }

    return std::make_unique<PaceMaker::Config>(RANGE_BEGIN_SECOND, RANGE_END_SECOND, SLEEP_INTERVAL_SECOND, dir);
}

PaceMaker::Config::Config(int64_t beginSecond_, int64_t endSecond_, int64_t intervalSecond_, const std::wstring &directory_)
    : beginSecond(beginSecond_)
    , endSecond(endSecond_)
    , intervalSecond(intervalSecond_)
    , directory(directory_)
{
}

PaceMaker::PaceMaker()
    : PaceMaker(PaceMaker::Config::Default())
{
}

PaceMaker::PaceMaker(std::unique_ptr<PaceMaker::Config> config)
    : m_Config(std::move(config))
{
    assert(m_Config);
    assert(m_Config->endSecond > m_Config->beginSecond);

    m_Rand = std::mt19937(m_RandomDevice());
}

bool PaceMaker::PrepareForSleeping()
{
    if (!m_Config)
    {
        LOG_WARN("The m_Config must not be null.");
        return false;
    }

    try
    {
        if (!boost::filesystem::exists(m_Config->directory))
        {
            boost::filesystem::create_directories(m_Config->directory);
        }

        int64_t currentSecond = GetCurrentSecondInUnixTime();

        auto duration = GenerateSecond();
        int64_t target = currentSecond + duration;

        // Write by to file
        boost::filesystem::path dstFilePath(m_Config->directory);
        dstFilePath /= UPDATE_DATETIME_TXT;

        std::wofstream dstFile(dstFilePath.wstring(), std::ios::out | std::ios::trunc);
        dstFile << target;
        dstFile.close();

        return true;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::out_of_range &e)
    {
        LOG_ERROR("Failed by std::out_of_range %s", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return false;
    }
}

bool PaceMaker::ResetSleep(const boost::filesystem::path &srcFilePath)
{
    try
    {
        LOG_INFO("Sleep %" PRId64 "[s] before preparing to avoid busy loop.", m_Config->intervalSecond);
        ::Sleep(static_cast<DWORD>(m_Config->intervalSecond) * 1000);

        RemoveFile(srcFilePath);

        if (!PrepareForSleeping())
        {
            LOG_WARN("PrepareForSleeping() failed.");
            return false;
        }

        return true;
    }
    catch (const boost::filesystem::filesystem_error &e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return false;
    }
}

int64_t PaceMaker::GenerateSecond()
{
    int64_t beginSecond = m_Config->beginSecond;
    int64_t endSecond = m_Config->endSecond;
    int64_t range = endSecond - beginSecond;
    return beginSecond + (m_Rand() % range);
}

PaceMaker::WaitResult PaceMaker::Wait(HANDLE eventObject)
{
    try
    {
        boost::filesystem::path srcFilePath(m_Config->directory);
        srcFilePath /= UPDATE_DATETIME_TXT;
        int64_t updateSecond = 0;

        while (true)
        {
            if (!boost::filesystem::exists(srcFilePath) && !PrepareForSleeping())
            {
                LOG_WARN("PrepareForSleeping() failed.");
                return PaceMaker::FAILED;
            }

            int64_t secondFromFile = 0;
            std::wifstream stream(srcFilePath.c_str());
            stream >> secondFromFile;
            stream.close();

            if (secondFromFile != updateSecond)  // This condition for avoiding verbose logs.
            {
                LOG_INFO("The updateScond is %" PRId64 "[s].", secondFromFile);
                updateSecond = secondFromFile;
            }

            if (updateSecond <= 0)
            {
                // Reach here when it is failed to read a value from file.
                LOG_WARN("The updateUnixTime is not expected value %" PRId64 ".", updateSecond);

                if (!ResetSleep(srcFilePath))
                {
                    LOG_WARN("ResetSleep() failed.");
                    return PaceMaker::FAILED;
                }
                continue;
            }

            int64_t currentSecond = GetCurrentSecondInUnixTime();
            int64_t durationSecond = updateSecond - currentSecond;
            if (durationSecond <= 0)
            {
                // Reach here when it is passed the duration time.
                LOG_INFO("The time to sleep is passed. %" PRId64 " - %" PRId64 " = %" PRId64, updateSecond, currentSecond, durationSecond);

                // Remove the file for next sleep.
                RemoveFile(srcFilePath);

                return PaceMaker::FINISHED_TO_SLEEP;
            }

            int64_t maxDurationSecond = m_Config->endSecond;
            if (durationSecond > maxDurationSecond)
            {
                // Reach here when it is need to reset the sleep time.
                LOG_WARN("The durationSecond is over maxDuration and generate time for sleep. %" PRId64 " > %" PRId64, durationSecond, maxDurationSecond);

                if (!ResetSleep(srcFilePath))
                {
                    LOG_WARN("ResetSleep() failed.");
                    return PaceMaker::FAILED;
                }
                continue;
            }

            DWORD sleepMilliSecond = static_cast<DWORD>((durationSecond >= m_Config->intervalSecond) ? m_Config->intervalSecond : durationSecond) * 1000;
            LOG_VERBOSE("Sleep for %u[ms]", sleepMilliSecond);

            DWORD r = WaitForSingleObject(eventObject, sleepMilliSecond);
            LOG_VERBOSE("WaitForSingleObject() done.");

            switch (r)
            {
            case WAIT_OBJECT_0:
                LOG_INFO("The m_EventStopControl is signaled.");
                return PaceMaker::GOT_EVENT;

            case WAIT_TIMEOUT:
                LOG_VERBOSE("Finish sleeping.");
                break;

            case WAIT_FAILED:
                LOG_ERROR("WaitForSingleObject() failed. %d", GetLastError());
                // Continue to stop sequence.
                return PaceMaker::FAILED;

            default:
                LOG_ERROR("WaitForSingleObject() returns unexpected code. %d", r);
                return PaceMaker::FAILED;
            }
        }
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return PaceMaker::FAILED;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return PaceMaker::FAILED;
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return PaceMaker::FAILED;
    }
}

int64_t PaceMaker::GetCurrentSecondInUnixTime() const
{
    return Utility::GetCurrentSecondInUnixTime();
}

void PaceMaker::RemoveFile(const boost::filesystem::path& path) const
{
    LOG_INFO("This programs about to delete file %s", path.string().c_str());

    boost::filesystem::remove(path);
}
