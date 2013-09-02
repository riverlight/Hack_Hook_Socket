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
	//�����׽���
	WORD myVersionRequest;
	WSADATA wsaData;
	myVersionRequest=MAKEWORD(1,1);
	int err;
	err=WSAStartup(myVersionRequest,&wsaData);
	if (!err)
	{
		printf("�Ѵ��׽���\n");
	}
	else
	{
		//��һ�����׽���
		printf("Ƕ����δ��!");
		return 1;
	}
	
	SOCKET serSocket=socket(AF_INET,SOCK_STREAM,0);//�����˿�ʶ���׽���
	//��Ҫ�󶨵Ĳ���
	SOCKADDR_IN addr;
	addr.sin_family=AF_INET; //��ַ��
	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip��ַ
	addr.sin_port=htons(5438);//�󶨶˿�
	
	bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));//�����
	listen(serSocket,5);//���еڶ������������ܹ����յ�����������
	
	//////////////////////////////////////////////////////////////////////////
	//��ʼ���м���
	//////////////////////////////////////////////////////////////////////////
	SOCKADDR_IN clientsocket;
	int len=sizeof(SOCKADDR);
	serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);
	
	/*
	char firstConn[100];
	sprintf(firstConn,"welcome %s!\nI got you!\n",inet_ntoa(clientsocket.sin_addr));//�ҶԶ�Ӧ��IP���ҽ������ִ�ӡ������
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
	
	closesocket(serConn);//�ر�
	WSACleanup();
	
	return 0;
}
