#include "pch.h"
#include <boost/filesystem.hpp>
#include <cpprest/uri_builder.h>
#include "InstallerFetcher.h"
#include "ConfigUtility.h"
#include <Log.h>

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const wchar_t BIZ_TELEWOR_MSI[] = L"BizTelework.msi";
}  // namespace

InstallerFetcher::Config InstallerFetcher::Config::Default()
{
    std::wstring dir = ConfigUtility::GetDataDirectory();
    if (dir.empty())
    {
        LOG_WARN("ConfigUtility::GetDataDirectory() failed.");
    }

    std::wstring url = ConfigUtility::GetUpdateUrl();
    if (url.empty())
    {
        LOG_WARN("ConfigUtility::GetUpdateUrl() failed.");
    }

    return InstallerFetcher::Config(dir, url);
}

InstallerFetcher::Config::Config(const std::wstring& destinationDirectory_, const std::wstring& downloadUrl_)
    : destinationDirectory(destinationDirectory_)
    , downloadUrl(downloadUrl_)
{
}

InstallerFetcher::InstallerFetcher(FetchingTaskInterface* fetchingTask, const InstallerFetcher::Config &config)
    : m_FetchingTask(fetchingTask)
    , m_Config(config)
{
}

bool InstallerFetcher::Fetch(std::wstring *dstMsiFilePath, HANDLE hEventStopControl)
{
    if (!dstMsiFilePath)
    {
        LOG_WARN("The dstMsiFilePath must not be null.");
        return false;
    }
    else if (!m_FetchingTask)
    {
        LOG_WARN("The m_FetchingTask must not be null.");
        return false;
    }
    else if (!hEventStopControl)
    {
        LOG_WARN("The hEventStopControl must not be null.");
        return false;
    }

    LOG_INFO("m_Config.destinationDirectory = %S.", m_Config.destinationDirectory.c_str());
    LOG_INFO("m_Config.downloadUrl = %S.", m_Config.downloadUrl.c_str());

    try
    {
        using web::uri_builder;

        boost::filesystem::path dir(m_Config.destinationDirectory);
        if (!boost::filesystem::exists(dir))
        {
            boost::filesystem::create_directories(dir);
        }

        boost::filesystem::path dstFile = dir / BIZ_TELEWOR_MSI;

        uri_builder builder(m_Config.downloadUrl);
        builder.append_path(BIZ_TELEWOR_MSI);

        auto url = builder.to_string();
        auto dst = dstFile.wstring();
        LOG_INFO("m_FetchingTask->RequestFile(%S, %S)...", url.c_str(), dst.c_str());
        if (!m_FetchingTask->RequestFile(url, dst, hEventStopControl))
        {
            LOG_INFO("m_FetchingTask->RequestFile() failed.");
            return false;
        }

        if (!boost::filesystem::exists(dstFile))
        {
            LOG_WARN("The installer file does not exist.");
            return false;
        }

        *dstMsiFilePath = dstFile.wstring();

        return true;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("client request task is failed. %s", e.what());
        return false;
    }
}
