#pragma once

#include <memory>
#include <Windows.h>
#include "UpdateChecker.h"
#include "InstallerFetcher.h"
#include "InstallerValidator.h"
#include "InstallerExecutor.h"

class UpdateProcess
{
public:
    UpdateProcess(std::unique_ptr<UpdateChecker> checker = std::make_unique<UpdateChecker>(),
                  std::unique_ptr<InstallerFetcher> fetcher = std::make_unique<InstallerFetcher>(),
                  std::unique_ptr<InstallerValidator> validator = std::make_unique<InstallerValidator>(),
                  std::unique_ptr<InstallerExecutor> executor = std::make_unique<InstallerExecutor>());
    virtual ~UpdateProcess() = default;

    virtual bool Execute(HANDLE hEventStopControl);

private:
    std::unique_ptr<UpdateChecker> m_UpdateChecker;
    std::unique_ptr<InstallerFetcher> m_InstallerFetcher;
    std::unique_ptr<InstallerValidator> m_InstallerValidator;
    std::unique_ptr<InstallerExecutor> m_InstallerExecutor;
};
