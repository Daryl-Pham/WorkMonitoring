
// BizTelework.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BizTelework.h"
#include "CompatApi.h"
#include <LanguageResource.h>
#define SENTRY_BUILD_STATIC 1
#include <sentry.h>
#include <boost/regex.hpp>
#include <cpprest/uri.h>
#include <map>
#include "BizTeleworkMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}  // namespace

// CBizTeleworkApp

BEGIN_MESSAGE_MAP(CBizTeleworkApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBizTeleworkApp construction

CBizTeleworkApp::CBizTeleworkApp(std::unique_ptr<BizTeleworkDependency> dependency)
    : m_Dependency(std::move(dependency))
{
}


// The one and only CBizTeleworkApp object

CBizTeleworkApp theApp;


// CBizTeleworkApp initialization

BOOL CBizTeleworkApp::InitInstance()
{
    InitLogger();

    if (!m_Dependency)
    {
        LOG_ERROR("The m_Dependency must not be null.");
        return FALSE;
    }

    DWORD timerIntervalCheckAuthen = m_Dependency->GetTimerIntervalCheckAuthen();
    DWORD timerIntervalCheckLog = m_Dependency->GetTimerIntervalCheckLog();
    std::wstring apiUrl = m_Dependency->GetApiUrl();

    LOG_INFO("timerIntervalCheckAuthen: %lu", timerIntervalCheckAuthen);
    LOG_INFO("timerIntervalCheckLog: %lu", timerIntervalCheckLog);
    LOG_INFO("apiUrl: %S", apiUrl.c_str());

    if (!ConfigureLanguageResource())
    {
        LOG_WARN("ConfigureLanguageResource() faield.");
    }

    if (CompatApi::HasSetThreadDpiAwarenessContext() && (!CompatApi::CallSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED)))
    {
        LOG_WARN("CompatApi::CallSetThreadDpiAwarenessContext() failed.");
    }

    CWinApp::InitInstance();

    InitTaskRunner();

    TypeUriRegexMatch type = TypeUriRegexMatch::REGEX_ONLY_BIZTELEWORK;
    ProcessUriScheme(&type);

    m_BizTeleworkInstance.reset(new BizTeleworkInstance());
    if (m_BizTeleworkInstance->CheckAlready())
    {
        LOG_INFO("This programs is already started on other process.");
        HWND hwnd = ::FindWindow(NULL, L"BizTelework");
        if (hwnd && !::SetForegroundWindow(hwnd))
        {
            LOG_WARN("SetForegroundWindow() failed");
        }
        if (hwnd && type == TypeUriRegexMatch::REGEX_AUTHENTICATE)
        {
            CopyData dataSend;
            dataSend.signature = Signature;
            dataSend.infor = m_ActiveInformation;
            COPYDATASTRUCT data = { COPYDATA_TYPE_1, sizeof(dataSend), &dataSend };
            ::SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&data);
        }
        return FALSE;
    }

    BOOL resultInitRichEdit = AfxInitRichEdit5();

    m_IoService.reset(new boost::asio::io_service());
    m_BizTeleworkInterval.reset(new BizTeleworkInterval(m_IoService.get(), timerIntervalCheckAuthen, timerIntervalCheckLog, apiUrl));
    auto routine = [this]()
    {
        try
        {
            m_IoService->run();
        }
        catch (const boost::system::system_error &e)
        {
            LOG_ERROR("Failed by boost::system::system_error %s", e.what());
            return;
        }
        catch (const std::exception &be)
        {
            LOG_ERROR("Failed by std::exception %s", be.what());
            return;
        }
        catch (...)
        {
            LOG_ERROR("Failed by an unknown exception");
            return;
        }
    };
    m_RunnerThread.reset(new std::thread(routine));

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    const CShellManager *pShellManager = new CShellManager;

    // Activate "Windows Native" visual manager for enabling themes in MFC controls
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    BizTeleworkMainWindowDependency dependency(apiUrl);
    m_BizTeleworkMainWindow.reset(new BizTeleworkMainWindow(dependency));
    if (!m_BizTeleworkMainWindow->Init(dependency))
    {
        LOG_ERROR("m_BizTeleworkMainWindow->Init() failed.");
        m_BizTeleworkMainWindow = nullptr;
        return FALSE;
    }
    m_pMainWnd = m_BizTeleworkMainWindow.get();

    if (!m_Dependency->IsActive())
    {
        m_Dependency->ShowAuthenticationDialog(m_ActiveInformation);
        LOG_INFO("BizTelework is not active.");
    }

    // Delete the shell manager created above.
    if (pShellManager != NULL)
    {
        delete pShellManager;
    }

    return TRUE;
}


int CBizTeleworkApp::ExitInstance()
{
    LOG_INFO("CBizTeleworkApp::ExitInstance()");

    ExitTaskRunner();

    if (m_BizTeleworkMainWindow && (!m_BizTeleworkMainWindow->PostMessage(WM_CLOSE)))
    {
        LOG_WARN("m_BizTeleworkMainWindow->PostMessage(WM_CLOSE) faield.");
    }

    if (m_IoService)
    {
        m_IoService->stop();
    }

    if (m_RunnerThread)
    {
        m_RunnerThread->join();
    }

    LoggerFinalize();

    return CWinApp::ExitInstance();
}

void CBizTeleworkApp::InitLogger() const
{
    try
    {
        boost::filesystem::wpath logDirectory = Utility::GetAppLogDirectory();
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
        LOG_WARN("Failed by boost::filesystem::filesystem_error %s", e.what());
    }
    catch (const std::exception& e)
    {
        LOG_WARN("Failed by std::exception %s", e.what());
    }
    catch (...)
    {
        LOG_WARN("Failed by an unknown exception");
    }
}

void CBizTeleworkApp::InitTaskRunner()
{
    if (m_RpcTaskRunner)
    {
        return;
    }

    m_RpcTaskRunner = std::make_unique<BizTeleworkUpdaterRpcTaskRunner>(std::make_unique<BizTeleworkUpdaterRpcClient>());
    if (!m_RpcTaskRunner->Start())
    {
        LOG_WARN("m_RpcTaskRunner->Start() failed.");
    }
}

void CBizTeleworkApp::ExitTaskRunner()
{
    if (m_RpcTaskRunner)
    {
        m_RpcTaskRunner->Shutdown();
    }
}

BOOL CBizTeleworkApp::ConfigureLanguageResource()
{
    auto lr = LanguageResource::Singleton();
    if (!lr)
    {
        LOG_WARN("LanguageResource::Singleton() failed.");
        return false;
    }
    else if (!lr->ReloadLanguageResource(LanguageIDDefault))
    {
        LOG_WARN("lr->ReloadLanguageResource(LanguageIDDefault) failed.");
        return false;
    }
    return true;
}

void CBizTeleworkApp::ProcessUriScheme(TypeUriRegexMatch* type)
{
    try
    {
        // Verify the TW is called from COM client
        std::wstring wStringCommandLine = std::wstring(m_lpCmdLine);
        if (wStringCommandLine.find(L"notification") != std::wstring::npos)
        {
            *type = TypeUriRegexMatch::REGEX_TOAST_NOTIFICATION;
            return;
        }

        web::uri bizTeleworkUri(m_lpCmdLine);
        if (bizTeleworkUri.host() == L"authenticate")
        {
            *type = TypeUriRegexMatch::REGEX_AUTHENTICATE;
            std::map<utility::string_t, utility::string_t> uriParameters = web::uri::split_query(bizTeleworkUri.query());
            if (uriParameters.find(L"company_code") != uriParameters.end())
            {
                std::wstring companyCode = uriParameters.at(L"company_code");
                wcscpy_s(m_ActiveInformation.companyCode, companyCode.length() + 1, &companyCode[0]);
            }
            if (uriParameters.find(L"activation_code") != uriParameters.end())
            {
                std::wstring activationCode = uriParameters.at(L"activation_code");
                wcscpy_s(m_ActiveInformation.activationCode, activationCode.length() + 1, &activationCode[0]);
            }
        }
    }
    catch (const web::uri_exception& uriException)
    {
        LOG_ERROR("Uri invalidate %s", uriException.what());
    }
    catch (const std::exception& exception)
    {
        LOG_ERROR("Error %s", exception.what());
    }
}
