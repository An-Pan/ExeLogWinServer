// ServerInstall.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

#define SVCNAME TEXT("LogWinServer")

int _tmain(int argc, _TCHAR* argv[])
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	schSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == schSCManager)
	{
		cout << "OpenSCManager failed (%d)" << GetLastError() << endl;
	}
	else{
		cout << "SCManager Opened" << endl;
	}

	// Create the service

	schService = CreateService(
		schSCManager,              // SCM database 
		SVCNAME,                   // name of service 
		SVCNAME,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		L"D:\\Project\\LogWinServer\\Debug\\LogWinServer.exe",                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
	{
		cout << "CreateService failed (%d)" << GetLastError() << endl;
		CloseServiceHandle(schSCManager);
		return -1;
	}
	else{
		cout << "Create Service Succeed." << endl;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

