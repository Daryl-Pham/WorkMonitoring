#include "pch.h"
#include <Log.h>
#include "UpdateProcess.h"

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}

UpdateProcess::UpdateProcess(std::unique_ptr<UpdateChecker> checker,
                             std::unique_ptr<InstallerFetcher> fetcher,
                             std::unique_ptr<InstallerValidator> validator,
                             std::unique_ptr<InstallerExecutor> executor)
    : m_UpdateChecker(std::move(checker))
    , m_InstallerFetcher(std::move(fetcher))
    , m_InstallerValidator(std::move(validator))
    , m_InstallerExecutor(std::move(executor))
{
}

bool UpdateProcess::Execute(HANDLE hEventStopControl)
{
    if (!hEventStopControl)
    {
        LOG_WARN("The hEventStopControl must not be null.");
        return false;
    }
    else if (!m_UpdateChecker)
    {
        LOG_WARN("The m_UpdateChecker must not be null.");
        return false;
    }
    else if (!m_InstallerFetcher)
    {
        LOG_WARN("The m_InstallerFetcher must not be null.");
        return false;
    }
    else if (!m_InstallerValidator)
    {
        LOG_WARN("The m_InstallerValidator must not be null.");
        return false;
    }
    else if (!m_InstallerExecutor)
    {
        LOG_WARN("The m_InstallerExecutor must not be null.");
        return false;
    }

    std::wstring msiFilePath;
    if (!m_UpdateChecker->CheckUpdating(hEventStopControl))
    {
        LOG_INFO("There is no update.");
        return false;
    }
    else if (!m_InstallerFetcher->Fetch(&msiFilePath, hEventStopControl))
    {
        LOG_INFO("m_InstallerFetcher->Fetch() failed.");
        return false;
    }

    LOG_INFO("The MSI file is destinated on %S", msiFilePath.c_str());

    // Reference https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_agent/-/blob/master/BizUpdater/Updater.cpp#L111-151
    // Validate MSI file.
    if (!m_InstallerValidator->ValidateMsiFile(msiFilePath))
    {
        LOG_WARN("ValidateMsiFile() failed.");
        return false;
    }
    // Verify signature of MSI file and itself app.
    else if (!m_InstallerValidator->ValidateSignature(msiFilePath))
    {
        LOG_WARN("VerifySignature() failed.");
        return false;
    }
    // Verify UpgradeCode of msi
    else if (!m_InstallerValidator->ValidateUpgradeCode(msiFilePath))
    {
        LOG_WARN("ValidateUpgradeCode() failed.");
        return false;
    }
    // Verify version.
    else if (!m_InstallerValidator->ValidateVersion(msiFilePath))
    {
        LOG_WARN("ValidateVersion() failed.");
        return false;
    }

    // Install the msi file.
    if (!m_InstallerExecutor->Execute(msiFilePath, hEventStopControl))
    {
        LOG_WARN("Execute() failed.");
        return false;
    }

    return true;
}
