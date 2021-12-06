#pragma once

#include <string>
#include "FetchingTaskInterface.h"

class InstallerFetcher
{
public:
    class Config
    {
    public:
        static Config Default();
        Config(const std::wstring& destinationDirectory, const std::wstring& downloadUrl);
        virtual ~Config() = default;

        const std::wstring destinationDirectory;
        const std::wstring downloadUrl;
    };

    explicit InstallerFetcher(FetchingTaskInterface* fetchingTask = nullptr, const Config &config = Config::Default());
    virtual ~InstallerFetcher() = default;

    virtual bool Fetch(std::wstring *dstMsiFilePath, HANDLE hEventStopControl);

private:
    FetchingTaskInterface* m_FetchingTask = nullptr;
    const Config m_Config;
};

