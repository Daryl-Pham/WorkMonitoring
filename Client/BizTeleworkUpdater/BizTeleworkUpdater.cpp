#include "pch.h"
#include "UpdaterServiceMain.h"
#include "InstallerExecutor.h"
#include <Utility.h>
#include <Log.h>

#ifdef BIZ_TELEWORK_UPDATER_TESTING
#error Do not define BIZ_TELEWORK_UPDATER_TESTING on BizTeleworkUpdater project because BIZ_TELEWORK_UPDATER_TESTING is for testing.
#endif

// Reference: http://eternalwindows.jp/windevelop/service/service00.html

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    void InitLogger()
    {
        try
        {
            boost::filesystem::wpath logDirectory = Utility::GetUpdaterServiceLogDirectory();
            if (logDirectory.empty())
            {
                LOG_WARN("Utility::GetDefaultDataDirectory() failed.");
            }
            else
            {
                LoggerInitialize(LOGGER_LOG_TYPE_INFO, logDirectory.wstring().c_str());
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed by std::exception %s", e.what());
        }
        catch (...)
        {
            LOG_ERROR("Failed by an unknown exception");
        }
    }
}  // namespace

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCmdShow)
{
    (void)hinst;        // Unused
    (void)hinstPrev;    // Unused
    (void)lpszCmdLine;  // Unused
    (void)nCmdShow;     // Unused

    InitLogger();
    InstallerExecutor::Initialize();

    wchar_t* serviceName = UpdaterServiceMain::SERVICE_NAME;
    SERVICE_TABLE_ENTRY serviceTable[] = {
        {serviceName, UpdaterServiceMain::ServiceMain},
        {NULL, NULL}
    };

    LOG_INFO("\"%S\" process is started.", serviceName);

    if (!StartServiceCtrlDispatcher(serviceTable))
    {
        LOG_WARN("StartServiceCtrlDispatcher() failed. %d", GetLastError());
    }

    InstallerExecutor::Finalize();
    LoggerFinalize();

    return 0;
}
