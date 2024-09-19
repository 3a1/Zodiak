#include "pch.h"

HANDLE dataHandle;

void MainThread() 
{
	while (true) 
	{
		/* Waiting for hl.exe */
		while (FindProcessByName(gProcessName, &gProcess) == STATUS_NOT_FOUND)
		{
			DebugPrint("waiting for -> %s", gProcessName);
			NtSleep(2500);
		}

		if (!gProcess) 
		{
			DebugPrint("gProcess -> not found");
			return;
		}
		DebugPrint("gProcess -> %p\n", gProcess);

		/* Getting the game PID */
		gPid = (ULONG64)PsGetProcessId(gProcess);

		if (!gPid) 
		{
			DebugPrint("gPid -> not found");
			return;
		}
		DebugPrint("gPid -> %i", (int)gPid);

		/* Pass execution to the data function */
		DataInitialize();
	}
}

NTSTATUS ZodiakEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) 
{
	UNREFERENCED_PARAMETER(driver_obj);
	UNREFERENCED_PARAMETER(registery_path);

	/* Microsoft compiler is sometimes retarded, thats why we have to do this non sense */
	/* It would otherwise generate wrapper functions around, and it would cause system BSOD */
	_KeAcquireSpinLockAtDpcLevel = (QWORD)KeAcquireSpinLockAtDpcLevel;
	_KeReleaseSpinLockFromDpcLevel = (QWORD)KeReleaseSpinLockFromDpcLevel;
	_IofCompleteRequest = (QWORD)IofCompleteRequest;
	_IoReleaseRemoveLockEx = (QWORD)IoReleaseRemoveLockEx;

	NTSTATUS status;

	/* Getting Windows Build Number */
	ULONG BuildNumber;
	PsGetVersion(NULL, NULL, &BuildNumber, NULL);
	DebugPrint("Windows Build: %u\n", BuildNumber);

	/* Setting offsets by build number */
    if (!SetOffsetsByBuildNumber(BuildNumber)) 
    {
        DebugPrint("Unsupported build number -> %lu\n", BuildNumber);
        return STATUS_NOT_SUPPORTED;
    }

	SpoofContext();
	/* Exporting Functions */
	if (!exports_init()) 
	{
		DebugPrint("Exports initialization failed");
		return STATUS_UNSUCCESSFUL;
	}
	RestoreContext();

	/* Starting Main Thread */
	status = PsCreateSystemThread(
		&dataHandle,
		THREAD_ALL_ACCESS,              
		NULL,                           
		NULL,                           
		NULL,                          
		(PKSTART_ROUTINE)MainThread,    
		NULL                            
	);
		
	if (!NT_SUCCESS(status)) 
	{
		DebugPrint("PsCreateSystemThread failed");
	}

	return status;
}
