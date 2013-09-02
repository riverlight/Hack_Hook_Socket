#ifndef _CSOCKET_H_
#define _CSOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h> 
#include <ctime>
#include"tlhelp32.h"

#include "stdafx.h"
#include "HSocketClientMain.h"


static FILE *log = fopen("C:\\log.txt", "a+");;

DWORD WINAPI RecvThread(LPVOID lp);
DWORD WINAPI SendThread(LPVOID lp);

DWORD WINAPI createSocketThread(LPVOID lp);



BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName);
BOOL RunProcess(LPCSTR lpImage);

void openHook();
bool judgeCommand( char *cmd );


#endif
