#include "spoof.h"

void SpoofContext() 
{
	currentProcess = IoGetCurrentProcess();
	currentThread = KeGetCurrentThread();
	memcpy(&currentCid, (PVOID)((char*)currentThread + cidOffset), sizeof(CLIENT_ID));

	/* Spoofing our process context to the win32 context */
	PVOID targetWin32Thread = 0;
	PETHREAD targetThread = GetWin32Thread(&targetWin32Thread);
	if (!targetWin32Thread || !targetThread)
	{
		DebugPrint("failed to find win32thread");
		return;
	}
	PEPROCESS targetProcess = PsGetThreadProcess(targetThread);

	CLIENT_ID targetCid = { 0 };
	memcpy(&targetCid, (PVOID)((char*)targetThread + cidOffset), sizeof(CLIENT_ID));

	KeStackAttachProcess(targetProcess, &apc);
	SpoofWin32Thread(targetWin32Thread, targetProcess, targetCid);
}

void RestoreContext() 
{
	SpoofWin32Thread(currentWin32Thread, currentProcess, currentCid);
	KeUnstackDetachProcess(&apc);
}

PETHREAD GetWin32Thread(PVOID* win32Thread)
{
	int currentThreadId = 1;
	NTSTATUS status = 0;
	do
	{
		PETHREAD currentEthread = 0;
		status = PsLookupThreadByThreadId((HANDLE)currentThreadId, &currentEthread);

		if (!NT_SUCCESS(status) || !currentEthread)
		{
			currentThreadId++;
			continue;
		}

		if (PsIsThreadTerminating(currentEthread))
		{
			currentThreadId++;
			continue;
		}

		PVOID Win32Thread;
		memcpy(&Win32Thread, (PVOID)((UINT64)currentEthread + win32ThreadOffset), sizeof(PVOID));

		if (Win32Thread)
		{
			PEPROCESS threadOwner = PsGetThreadProcess(currentEthread);
			char procName[15];
			memcpy(&procName, (PVOID)((UINT64)threadOwner + imageFileNameOffset), sizeof(procName));
			if (!strcmp(procName, "dwm.exe")) // explorer.exe causing bsod
			{
				*win32Thread = Win32Thread;
				return currentEthread;
			}
		}
		currentThreadId++;
	} while (0x3000 > currentThreadId);

	return 0;
}

void SpoofWin32Thread(PVOID newWin32Value, PEPROCESS newProcess, CLIENT_ID newClientId)
{
	PVOID win32ThreadPtr = (PVOID)((char*)currentThread + win32ThreadOffset);
	memcpy(win32ThreadPtr, &newWin32Value, sizeof(PVOID));

	PVOID processPtr = (PVOID)((char*)currentThread + processOffset);
	memcpy(processPtr, &newProcess, sizeof(PEPROCESS));

	PVOID clientIdPtr = (PVOID)((char*)currentThread + cidOffset);
	memcpy(clientIdPtr, &newClientId, sizeof(CLIENT_ID));
}

bool SetOffsetsByBuildNumber(ULONG buildNumber)
{
    /* Set offsets based on the build number */

    if (buildNumber >= 22621) // Windows 11 22H2
    {
        processOffset = 0x450;
        win32ThreadOffset = 0x1c8;
        cidOffset = 0x7a0;
        imageFileNameOffset = 0x5a8;
        return true;
    }
    else if (buildNumber >= 22000) // Windows 11 21H2
    {
        processOffset = 0x448;
        win32ThreadOffset = 0x1c8;
        cidOffset = 0x7a0;
        imageFileNameOffset = 0x5a8;
        return true;
    }
    else if (buildNumber >= 19041) // Windows 10 2004, 20H2, 21H1, 21H2
    {
        processOffset = 0x220;
        win32ThreadOffset = 0x1c8;
        cidOffset = 0x478;
        imageFileNameOffset = 0x5a8;
        return true;
    }
    else if (buildNumber >= 18362) // Windows 10 1903, 1909
    {
        processOffset = 0x218;
        win32ThreadOffset = 0x1b8;
        cidOffset = 0x470;
        imageFileNameOffset = 0x598;
        return true;
    }
    else if (buildNumber >= 17763) // Windows 10 1809
    { 
        processOffset = 0x218;
        win32ThreadOffset = 0x1b8;
        cidOffset = 0x478;
        imageFileNameOffset = 0x5a8;
        return true;
    }
    else if (buildNumber >= 17134) // Windows 10 1803
    { 
        processOffset = 0x210;
        win32ThreadOffset = 0x1b0;
        cidOffset = 0x460;
        imageFileNameOffset = 0x588;
        return true;
    }
    else if (buildNumber >= 16299) // Windows 10 1709
    { 
        processOffset = 0x208;
        win32ThreadOffset = 0x1a8;
        cidOffset = 0x450;
        imageFileNameOffset = 0x578;
        return true;
    }
    else if (buildNumber >= 15063) // Windows 10 1703
    { 
        processOffset = 0x200;
        win32ThreadOffset = 0x1a0;
        cidOffset = 0x448;
        imageFileNameOffset = 0x570;
        return true;
    }
    else if (buildNumber >= 14393) // Windows 10 1607
    { 
        processOffset = 0x198;
        win32ThreadOffset = 0x190;
        cidOffset = 0x440;
        imageFileNameOffset = 0x568;
        return true;
    }
    else if (buildNumber >= 10586) // Windows 10 1511
    { 
        processOffset = 0x190;
        win32ThreadOffset = 0x188;
        cidOffset = 0x438;
        imageFileNameOffset = 0x560;
        return true;
    }
    else if (buildNumber >= 10240) // Windows 10 1507
    { 
        processOffset = 0x188;
        win32ThreadOffset = 0x180;
        cidOffset = 0x430;
        imageFileNameOffset = 0x558;
        return true;
    }
    else 
    {
        return false;
    }
}