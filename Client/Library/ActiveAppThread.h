#pragma once
class CActiveAppThread
{

public:
    CActiveAppThread();
    virtual ~CActiveAppThread();

    DWORD GetErrorCode() { return m_dwErrorCode; };

    BOOL Start(SEND_COLLECT_LOG_INFO info);
    BOOL Stop();

    void Suspend();
    void Resume();

    BOOL IsRun();
    BOOL IsCancel();
    void Wait();

    virtual UINT GetActiveLogProc();

private:
    CWinThread* m_pThread;
    BOOL        m_bCancel;
    CEvent      m_eventSuspend;

};