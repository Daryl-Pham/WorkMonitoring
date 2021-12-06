#include "pch.h"
#include "UpdaterServiceMain.h"
#include "UpdaterService.h"
#include "UpdaterServiceInterface.h"
#include "AppExecutor.h"
#include "BizTeleworkUpdaterRpcServer.h"
#include <memory>
#include <Log.h>


namespace
{
    DEFINE_LOGGER_FILE_NAME;

    std::unique_ptr<UpdaterServiceInterface> updaterService = nullptr;
    std::unique_ptr<BizTeleworkUpdaterRpcServer> rpcServer = nullptr;
    std::unique_ptr<AppExecutor> appExecutor = nullptr;
}


LPWSTR UpdaterServiceMain::SERVICE_NAME = L"OptimalBizTeleworkUpdater";

VOID WINAPI UpdaterServiceMain::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    (void)dwArgc;    // Unused
    (void)lpszArgv;  // Unused

    try
    {
        if (!rpcServer)
        {
            rpcServer = std::make_unique<BizTeleworkUpdaterRpcServer>();
        }

        if (!updaterService)
        {
            auto dep = std::make_unique<UpdaterService::Dependency>(rpcServer.get());
            updaterService = std::make_unique<UpdaterService>(std::move(dep));
        }

        if (!appExecutor)
        {
            appExecutor = std::make_unique<AppExecutor>();
        }
    }
    catch (std::bad_alloc e)
    {
        LOG_ERROR("std::make_unique() failed. %s", e.what());
        return;
    }

    appExecutor->ExecuteOnEachSession();

    rpcServer->Start();

    if (!updaterService->PreRun(SERVICE_NAME))
    {
        LOG_WARN("updaterService->PreRun() failed.");
        return;
    }
    else if (!updaterService->Run())
    {
        LOG_WARN("updaterService->Run() failed.");
        return;
    }

    rpcServer->Shutdown();

    LOG_INFO("ServiceMain() is done.");
}

#ifdef BIZ_TELEWORK_UPDATER_TESTING
void UpdaterServiceMain::SetUpdaterService(std::unique_ptr<UpdaterServiceInterface> updaterService_)
{
    updaterService = std::move(updaterService_);
}
void UpdaterServiceMain::SetAppExecutor(std::unique_ptr<AppExecutor> appExecutor_)
{
    appExecutor = std::move(appExecutor_);
}
#endif  // BIZ_TELEWORK_UPDATER_TESTING
