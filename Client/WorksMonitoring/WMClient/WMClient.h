
// WMClient.h : main header file for the WMClient application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <memory>


// CWMClientApp:
// See WMClient.cpp for the implementation of this class
//

class WMMainWindow;

class CWMClientApp : public CWinApp
{
public:
	CWMClientApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	std::unique_ptr<WMMainWindow> m_WMMainWindow;
};

extern CWMClientApp theApp;
