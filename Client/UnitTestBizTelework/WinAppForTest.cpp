#include "WinAppForTest.h"


BOOL WinAppForTest::InitInstance()
{
    CWinApp::InitInstance();

    m_Wnd = m_OnInitInstance();
    if (!m_Wnd)
    {
        return FALSE;
    }

    m_pMainWnd = m_Wnd.get();

    if (m_IsDialog)
    {
        reinterpret_cast<CDialogEx*>(m_Wnd.get())->DoModal();
        return FALSE;
    }
    return TRUE;
}
