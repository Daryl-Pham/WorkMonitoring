
// WMClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "WMClient.h"

#include "WMMainWindow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWMClientApp

BEGIN_MESSAGE_MAP(CWMClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CWMClientApp::OnAppAbout)
END_MESSAGE_MAP()


// CWMClientApp construction

CWMClientApp::CWMClientApp()
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("WMClient.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CWMClientApp object

CWMClientApp theApp;


// CWMClientApp initialization

BOOL CWMClientApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	m_WMMainWindow.reset(new WMMainWindow());
	if (!m_WMMainWindow->Init())
	{
		m_WMMainWindow = nullptr;
		return FALSE;
	}
	m_pMainWnd = m_WMMainWindow.get();

	return TRUE;
}

int CWMClientApp::ExitInstance()
{
	if (m_WMMainWindow && (!m_WMMainWindow->PostMessage(WM_CLOSE)))
	{
		
	}
	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

// CWMClientApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CWMClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CWMClientApp message handlers



