#include "HSocketServerMain.h"

SOCKET serConn;


CRITICAL_SECTION gcs;

DWORD WINAPI RecvThread(LPVOID lp);
DWORD WINAPI SendThread(LPVOID lp);

DWORD WINAPI RecvThread(LPVOID lp)
{
	int num=*(DWORD*)lp;
	
	char receiveBuf[100];
	while(1)
	{
		int i = recv(serConn,receiveBuf,101,0);
		
		if(i>0)
        {
		  printf("client:\n  %s\n",receiveBuf);
        }
	}
	return 0;
}


DWORD WINAPI SendThread(LPVOID lp)
{
	int num=*(DWORD*)lp;

	while(1)
	{
		string tmp;
		getline(cin, tmp);
		printf("%s", tmp.c_str());

		send( serConn, tmp.c_str(), tmp.length() + 1, 0 );
	}
	
	return 0;
}

int main()
{
	//创建套接字
	WORD myVersionRequest;
	WSADATA wsaData;
	myVersionRequest=MAKEWORD(1,1);
	int err;
	err=WSAStartup(myVersionRequest,&wsaData);
	if (!err)
	{
		printf("已打开套接字\n");
	}
	else
	{
		//进一步绑定套接字
		printf("嵌套字未打开!");
		return 1;
	}
	
	SOCKET serSocket=socket(AF_INET,SOCK_STREAM,0);//创建了可识别套接字
	//需要绑定的参数
	SOCKADDR_IN addr;
	addr.sin_family=AF_INET; //地址簇
	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip地址
	addr.sin_port=htons(5438);//绑定端口
	
	bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));//绑定完成
	listen(serSocket,5);//其中第二个参数代表能够接收的最多的连接数
	
	//////////////////////////////////////////////////////////////////////////
	//开始进行监听
	//////////////////////////////////////////////////////////////////////////
	SOCKADDR_IN clientsocket;
	int len=sizeof(SOCKADDR);
	serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);
	
	/*
	char firstConn[100];
	sprintf(firstConn,"welcome %s!\nI got you!\n",inet_ntoa(clientsocket.sin_addr));//找对对应的IP并且将这行字打印到那里
	send(serConn,firstConn,strlen(firstConn)+1,0);
	*/
	
	
	InitializeCriticalSection(&gcs);
	HANDLE Thread[2];
	
	int num = 0;
	Thread[0]=CreateThread( NULL,0,RecvThread, &num, 0,NULL); 
	Sleep(500);
	
	num = 1;
	Thread[1]=CreateThread( NULL,0,SendThread, &num, 0,NULL); 
	Sleep(500);
	
	
	WaitForMultipleObjects(2,Thread,TRUE,100000);
	
	DeleteCriticalSection(&gcs);
	CloseHandle(Thread);
	
	closesocket(serConn);//关闭
	WSACleanup();
	
	return 0;
}
