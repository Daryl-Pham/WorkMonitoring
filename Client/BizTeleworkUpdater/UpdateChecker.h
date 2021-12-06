#pragma once

#include <memory>
#include <string>
#include "AppVersion.h"
#include "FetchingTaskInterface.h"

class UpdateChecker
{
public:
    class Config
    {
    public:
        static std::unique_ptr<Config> Default();

        Config(const std::wstring& url_, const AppVersion& appVersion_);
        virtual ~Config() = default;

        const std::wstring url;
        const AppVersion appVersion;
    };

    explicit UpdateChecker(std::unique_ptr<UpdateChecker::Config> config, FetchingTaskInterface* fetchingTaskProducer);
    virtual ~UpdateChecker() = default;

    /**
     * @return True if this app has updating. False if no update availables.
     */
    virtual bool CheckUpdating(HANDLE hEventStopControl);

private:
    const std::unique_ptr<UpdateChecker::Config> m_Config;
    FetchingTaskInterface* m_FetchingTask;
};
