
// RemoteWork.h : main header file for the PROJECT_NAME application
//

#pragma once

#include <memory>
#include <string>
#include <thread>
#include "BizTeleworkDependency.h"
#include "BizTeleworkInterval.h"
#include "BizTeleworkMainWindow.h"
#include "BizTeleworkInstance.h"
#include "BizTeleworkUpdaterRpcTaskRunner.h"
#include "CopyDataForAuthenticationUri.h"

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

enum class TypeUriRegexMatch
{
    REGEX_AUTHENTICATE,
    REGEX_ONLY_BIZTELEWORK,
    REGEX_TOAST_NOTIFICATION,
};

// CBizTeleworkApp:
// See RemoteWork.cpp for the implementation of this class
//

class CBizTeleworkApp : public CWinApp
{
public:
    explicit CBizTeleworkApp(std::unique_ptr<BizTeleworkDependency> dependency = std::make_unique<BizTeleworkDependency>());

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
    void InitLogger() const;
    void InitTaskRunner();
    void ExitTaskRunner();
    BOOL ConfigureLanguageResource();
    void ProcessUriScheme(TypeUriRegexMatch* type);

    const std::unique_ptr<BizTeleworkDependency> m_Dependency;
    std::unique_ptr<boost::asio::io_service> m_IoService;
    std::unique_ptr<std::thread> m_RunnerThread;
    std::unique_ptr<BizTeleworkInterval> m_BizTeleworkInterval;
    std::unique_ptr<BizTeleworkMainWindow> m_BizTeleworkMainWindow;
    std::unique_ptr<BizTeleworkInstance> m_BizTeleworkInstance;
    ActiveInformation m_ActiveInformation = {};
    std::unique_ptr<BizTeleworkUpdaterRpcTaskRunner> m_RpcTaskRunner;
};

extern CBizTeleworkApp theApp;
