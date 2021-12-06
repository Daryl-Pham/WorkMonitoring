#include "pch.h"
#include "ConfigUtility.h"
#include <AppConfig.h>
#include <Utility.h>
#include <Log.h>

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}  // namespace

std::wstring ConfigUtility::GetDataDirectory()
{
    try
    {
        auto dir = Utility::CreateProgramDataDirectory();
        std::wstring dirStr = dir.wstring();
        if (dirStr.empty())
        {
            LOG_WARN("The dirStr must not be empty.");
            return L"";
        }

        return dirStr;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}

boost::filesystem::path ConfigUtility::GetLogDirectory()
{
    try
    {
        auto dir = Utility::GetUpdaterServiceLogDirectory();
        if (dir.empty())
        {
            LOG_WARN("Utility::GetUpdaterServiceLogDirectory() failed.");
        }

        return dir;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return L"";
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}

std::wstring ConfigUtility::GetUpdateUrl()
{
    try
    {
        std::wstring updateUrl;
        AppConfig appConfig;

        if (!appConfig.GetUpdateUrl(&updateUrl))
        {
            LOG_WARN("appConfig.GetApiUrl() failed.");
            return L"";
        }

        return updateUrl;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}

std::wstring ConfigUtility::GetUpgradeCode()
{
    try
    {
        std::wstring upgradeCode;
        AppConfig appConfig;

        if (!appConfig.GetUpgradeCode(&upgradeCode))
        {
            LOG_WARN("appConfig.GetUpgradeCode() failed.");
            return L"";
        }

        return upgradeCode;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}

std::wstring ConfigUtility::GetVersion()
{
    try
    {
        std::wstring version;
        AppConfig appConfig;

        if (!appConfig.GetVersion(&version))
        {
            LOG_WARN("appConfig.GetVersion() failed.");
            return L"";
        }

        return version;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}


std::wstring ConfigUtility::GetBizTeleworkUpdaterRPCEndpoint()
{
    try
    {
        std::wstring endpoint;
        AppConfig appConfig;

        if (!appConfig.GetBizTeleworkUpdaterRPCEndpoint(&endpoint))
        {
            LOG_WARN("appConfig.GetBizTeleworkUpdaterRPCEndpoint() failed.");
            return L"";
        }

        return endpoint;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}
