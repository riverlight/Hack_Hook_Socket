#include "cSocket.h"
#include "stdafx.h"
#include"tlhelp32.h"

#define SERVICE_NAME "srv_demo"

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
void WINAPI service_main(int argc, char** argv); 
void WINAPI ServiceHandler(DWORD fdwControl);

TCHAR szSvcName[80];
SC_HANDLE schSCManager;
SC_HANDLE schService;
int uaquit;


void WINAPI ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
		ServiceStatus.dwCheckPoint    = 0; 
		ServiceStatus.dwWaitHint      = 0;
		uaquit= 1;
		//add you quit code here
		if(log != NULL)
			fclose(log);
		break; 
	default:
		return;	
	};
	if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		DWORD nError = GetLastError();
	} 
}


void WINAPI service_main(int argc, char** argv) 
{       
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    hServiceStatusHandle = RegisterServiceCtrlHandler(_T(SERVICE_NAME), ServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		DWORD nError = GetLastError();
    }  
	//add your init code here
   
	//-------------add your service thread here
    
	
	HANDLE task_handle = CreateThread(NULL,NULL,createSocketThread,NULL,NULL,NULL);
	Sleep(500);
	
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 9000;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		DWORD nError = GetLastError();
    } 
	
} 
//do not change main function
int main (int argc, const char *argv[])
{
    SERVICE_TABLE_ENTRY ServiceTable[2];
    
    ServiceTable[0].lpServiceName = _T(SERVICE_NAME);
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)service_main;
    
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    // 启动服务的控制分派机线程
    StartServiceCtrlDispatcher(ServiceTable); 
    return 0;
}

