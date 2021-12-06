#pragma once
int AppLogThreadsInit(DWORD ThreadId);
int AppLogThreadsStart(LPVOID param);
int AppLogThreadsSendMsg(UINT msg, DWORD id);		// STOP‚ðŠÜ‚Þ
int AppLogThreadsSuspend(LPVOID param);
int AppLogThreadsResume(LPVOID param);