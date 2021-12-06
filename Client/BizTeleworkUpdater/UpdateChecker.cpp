#include "pch.h"
#include <Log.h>
#include <cpprest/uri_builder.h>
#include "ConfigUtility.h"
#include "UpdateChecker.h"

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const wchar_t *const PATH_WINDOWS_VERSION_TXT = L"/windows/version.txt";

}  // namespace

std::unique_ptr<UpdateChecker::Config> UpdateChecker::Config::Default()
{
    std::wstring updateUrl = ConfigUtility::GetUpdateUrl();
    LOG_TAG_INFO("UpdateCheckerConfig.h", "ConfigUtility::GetUpdateUrl() returns %S", updateUrl.c_str());
    std::wstring v = ConfigUtility::GetVersion();
    LOG_TAG_INFO("UpdateCheckerConfig.h", "ConfigUtility::GetVersion() returns %S", v.c_str());
    auto appVersion = AppVersion::Parse(v);
    if (!appVersion)
    {
        LOG_TAG_ERROR("UpdateCheckerConfig.h", "AppVersion::Parse() failed.");
        return nullptr;
    }

    return std::make_unique<UpdateChecker::Config>(updateUrl, *appVersion);
};

UpdateChecker::Config::Config(const std::wstring& url_, const AppVersion& appVersion_)
    : url(url_), appVersion(appVersion_)
{
}

UpdateChecker::UpdateChecker(std::unique_ptr<UpdateChecker::Config> config, FetchingTaskInterface* fetchingTask)
    : m_Config(std::move(config))
    , m_FetchingTask(fetchingTask)
{
}

bool UpdateChecker::CheckUpdating(HANDLE hEventStopControl)
{
    if (!m_Config)
    {
        LOG_WARN("The m_Config must not be null.");
        return false;
    }
    else if (!m_FetchingTask)
    {
        LOG_WARN("The m_FetchingTask must not be null.");
        return false;
    }

    LOG_INFO("m_Config->url = %S.", m_Config->url.c_str());

    try
    {
        using web::uri_builder;

        uri_builder builder(m_Config->url);
        builder.append_path(PATH_WINDOWS_VERSION_TXT);
        std::wstring body;
        if (!m_FetchingTask->RequestBodyString(builder.to_string(), &body, hEventStopControl))
        {
            LOG_WARN("m_FetchingTask->RequestBodyString() failed.");
            return false;
        }

        auto remoteAppVersion = AppVersion::Parse(body);
        if (!remoteAppVersion)
        {
            LOG_WARN("AppVersion::Parse() failed.");
            LOG_WARN("The body is %S", body.c_str());
            return false;
        }

        return remoteAppVersion->IsNewerThan(m_Config->appVersion);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("client request task is failed. %s", e.what());
        return false;
    }
}
