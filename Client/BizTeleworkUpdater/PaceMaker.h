#pragma once

#include <memory>
#include <random>
#include <Windows.h>
#include <boost/filesystem.hpp>

class PaceMaker
{
public:
    typedef enum
    {
        FINISHED_TO_SLEEP,
        GOT_EVENT,
        FAILED,
    } WaitResult;

    class Config
    {
    public:
        static std::unique_ptr<Config> Default();

        Config(int64_t beginSecond, int64_t endSecond, int64_t intervalSecond, const std::wstring &directory);
        virtual ~Config() = default;

        const int64_t beginSecond;
        const int64_t endSecond;
        const int64_t intervalSecond;
        const std::wstring directory;
    };

    PaceMaker();
    explicit PaceMaker(std::unique_ptr<PaceMaker::Config> config);
    virtual ~PaceMaker() = default;

    virtual WaitResult Wait(HANDLE eventObject);

private:
    int64_t GenerateSecond();
    bool PrepareForSleeping();
    bool ResetSleep(const boost::filesystem::path &srcFilePath);
    int64_t GetCurrentSecondInUnixTime() const;
    void RemoveFile(const boost::filesystem::path& path) const;

    std::unique_ptr<PaceMaker::Config> m_Config;
    std::random_device m_RandomDevice;
    std::mt19937 m_Rand;
};
