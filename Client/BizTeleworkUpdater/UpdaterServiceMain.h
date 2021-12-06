#pragma once


#ifdef BIZ_TELEWORK_UPDATER_TESTING
#include <memory>
#include "UpdaterServiceInterface.h"
#include "AppExecutor.h"
#endif  // BIZ_TELEWORK_UPDATER_TESTING

class UpdaterServiceMain
{
public:
    static VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static LPWSTR SERVICE_NAME;

#ifdef BIZ_TELEWORK_UPDATER_TESTING
    static void SetUpdaterService(std::unique_ptr<UpdaterServiceInterface> updaterService_);
    static void SetAppExecutor(std::unique_ptr<AppExecutor> appExecutor_);
#endif  // BIZ_TELEWORK_UPDATER_TESTING
};
