#include "stdafx.h"
#include "WorkerThread.h"
#include "BizTelework.h"
#include "BizTeleworkThreadAPI.h"

int BizTeleworkThreadsInit(DWORD ThreadId)
{
    /*AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CBizTeleworkThreadApp* pApp = (CBizTeleworkThreadApp*)AfxGetApp();
    pApp->SetServiceThreadId(ThreadId);

    return pApp->InitWorkerThreads();*/
    return 1;
}

int BizTeleworkThreadsStart(LPVOID param)
{
    CBizTeleworkApp* pApp = (CBizTeleworkApp*)AfxGetApp();

    /*return(pApp->StartWorkerThreads());*/
    return 1;
}

int BizTeleworkThreadsSendMsg(UINT msg, DWORD id)
{

    CBizTeleworkApp* pApp = (CBizTeleworkApp*)AfxGetApp();
    /*if (msg == MM_STOP_THREAD || msg == MM_SESSION_CONNECT || msg == MM_SESSION_DISCONNECT)
    {
    pApp->SendMsgWorkerThreads(msg, id);
    return 0;
    }*/

    return 1;
}

int BizTeleworkThreadsSuspend(LPVOID param)
{

    return 0;
}

int BizTeleworkThreadsResume(LPVOID param)
{

    return 0;
}