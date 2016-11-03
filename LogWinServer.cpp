// LogWinServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <atlstr.h>
#include <iostream>
#include <fstream>
#include <strstream>
using namespace std;

#define SVCNAME TEXT("LogWinServer")


fstream logFile;
SERVICE_STATUS gSvcStatus;
SERVICE_STATUS_HANDLE gSvcStatusHandle;
HANDLE ghSvcStopEvent = NULL;


VOID WRITE_LOG(string log)
{
	SYSTEMTIME Time;
	GetLocalTime(&Time);

	string strTime;
	
	if (!logFile.is_open())
		return;
	logFile << Time.wYear << "-" << Time.wMonth << "-" << Time.wDay << " " << Time.wHour << ":" << Time.wMinute << ":" << Time.wMilliseconds << "   ";
	logFile << log.c_str() << endl;
	logFile.flush();


}
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl);
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv);
int _tmain(int argc, _TCHAR* argv[])
{
	logFile.open("D:\\Project\\LogWinServer\\Log.txt", ios::app);  //  Open 与 Close 必须成对出现

	SERVICE_TABLE_ENTRY DispathcTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};
	BOOL success;
	success = StartServiceCtrlDispatcher(DispathcTable);
	if (!success)
	{
		//error occured
		WRITE_LOG("DispathcTable error");
	}
	else
	{
		WRITE_LOG("Service DIspatch");
	}

	return 0;
}

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	//RegisterServiceCtrlHandler should be the first nonfailing function 
	// in ServiceMain so the service can use the status handle returned by 
	// this function to call SetServiceStatus with the SERVICE_STOPPED state if an error occurs
	//WRITE_LOG("Start SvcMain.");
	gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);

	if (!gSvcStatusHandle)
	{
		WRITE_LOG("RegisterServiceCtrlHandler Failed.");
		return;
	}
	else{
		WRITE_LOG("RegisterServiceCtrlHandler succeed.");
	}
	// These SERVICE_STATUS members remain as set here
	gSvcStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	WRITE_LOG("Update Service state to Pending");

	SvcInit(dwArgc, lpszArgv);
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_ACCEPT_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.
		SetEvent(ghSvcStopEvent);
		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}
}

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else
		gSvcStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);


}

VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
	WRITE_LOG("SvcInit....");
	ghSvcStopEvent = CreateEvent(NULL
		, TRUE,
		FALSE,		// not signaled
		NULL);		// no name

	if (ghSvcStopEvent == NULL)
	{
		WRITE_LOG("CreateEvent error");
		return;
	}

	WRITE_LOG("Update Service state to Running");
	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	do {
		
		Sleep(1000 * 30);
		WRITE_LOG("Service is working");

	} while (WaitForSingleObject(ghSvcStopEvent, 1000) != WAIT_OBJECT_0);

	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}