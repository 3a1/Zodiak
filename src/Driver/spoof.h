#pragma once
#include "pch.h"

/* Offsets */
ULONG processOffset; //_KTHREAD->_KPROCESS* Process;
ULONG win32ThreadOffset; //_KTHREAD->VOID* volatile Win32Thread
ULONG cidOffset; //_ETHREAD->_CLIENT_ID Cid;  
ULONG imageFileNameOffset; //_PEPROCESS->UCHAR ImageFileName[15];   

KAPC_STATE apc;
PVOID currentWin32Thread;
PEPROCESS currentProcess;
PETHREAD currentThread;
CLIENT_ID currentCid;

void SpoofContext();
void RestoreContext();

/* Function that change offsets based on windows build number */
bool SetOffsetsByBuildNumber(ULONG buildNumber);

/* Local functions */
static PETHREAD GetWin32Thread(PVOID* win32Thread);
static void SpoofWin32Thread(PVOID newWin32Value, PEPROCESS newProcess, CLIENT_ID newClientId);

