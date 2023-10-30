#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>

#define DRIVER_NAME		L"\\Device\\top_ring0"
#define SYMBOL_NAME		L"\\DosDevices\\top_ring0"

#define CLIENT_DLL	    L"\\cstrike\\bin\\client.dll"


#define IO_INIT_CHEAT_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_MODULES					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriver, IN PUNICODE_STRING RegistryPath);
void DriverUnload(IN DRIVER_OBJECT pDriver);

HANDLE CSGO_ID;
HANDLE CSGO_PROCESS;
DWORD32 CLIENT_DLL_BASE;
HANDLE CHEAT_ID;
HANDLE CHEAT_PROCESS;

UNICODE_STRING DeviceName;
UNICODE_STRING DosName;
PDEVICE_OBJECT pDeviceObj;

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BifferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WrittenBytes);

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	return MmCopyVirtualMemory(Process, (PVOID)SourceAddress, CHEAT_PROCESS, (PVOID)TargetAddress, Size, KernelMode, ReadedBytes);
};


NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WrittenBytes)
{
	return MmCopyVirtualMemory(CHEAT_PROCESS, (PVOID)TargetAddress, Process, (PVOID)SourceAddress, Size, KernelMode, WrittenBytes);
};

typedef struct _KERNEL_INIT_DATA_REQUEST
{
	DWORD32 gameId;
	DWORD32 cheatId;
	NTSTATUS result;
} KERNEL_INIT_DATA_REQUEST, * PKERNEL_INIT_DATA_REQUEST;


typedef struct _KERNEL_GET_MODULES_REQUEST
{
	DWORD32 bClient;
	NTSTATUS result;
} KERNEL_GET_MODULES_REQUEST, * PKERNEL_GET_MODULES_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	DWORD32 Address;
	DWORD64 Response;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;


typedef struct _KERNEL_WRITE_REQUEST
{
	DWORD32 Address;
	DWORD64 Value;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;


NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObjcet, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObjcet, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS IoControl(PDEVICE_OBJECT DeviceObjcet, PIRP Irp)
{
	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	PKERNEL_WRITE_REQUEST pWriteRequest;
	PKERNEL_READ_REQUEST pReadRequest;
	SIZE_T rwBytes = 0;

	switch (controlCode)
	{

	case IO_INIT_CHEAT_DATA:
		void InitCheatData(PIRP Irp);
		InitCheatData(Irp);
		bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
		break;

	case IO_READ_PROCESS_MEMORY:
		pReadRequest = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if (pReadRequest) {
			pReadRequest->Result = KeReadVirtualMemory(CSGO_PROCESS, pReadRequest->Address, pReadRequest->Response, pReadRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST);
		break;

	case IO_WRITE_PROCESS_MEMORY:

		pWriteRequest = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if (pWriteRequest)
		{
			pWriteRequest->Result = KeWriteVirtualMemory(CSGO_PROCESS, pWriteRequest->Address, pWriteRequest->Value, pWriteRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_WRITE_REQUEST);
		break;

	case IO_GET_MODULES:
		void GetAllModules(PIRP Irp);
		GetAllModules(Irp);

		bytesIO = sizeof(KERNEL_GET_MODULES_REQUEST);
		break;

	default:

		break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = bytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

void InitCheatData(PIRP Irp)
{
	PKERNEL_INIT_DATA_REQUEST pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
	if (pInitData)
	{
		CSGO_ID = (HANDLE)pInitData->gameId;
		CHEAT_ID = (HANDLE)pInitData->cheatId;


		pInitData->result = PsLookupProcessByProcessId(CSGO_ID, &CSGO_PROCESS);
		pInitData->result |= PsLookupProcessByProcessId(CHEAT_ID, &CHEAT_PROCESS);
	}
}

void GetAllModules(PIRP Irp)
{
	PKERNEL_GET_MODULES_REQUEST pModules = (PKERNEL_GET_MODULES_REQUEST)Irp->AssociatedIrp.SystemBuffer;
	if (pModules)
	{
		if (CLIENT_DLL_BASE == 0)
			pModules->result = STATUS_NOT_FOUND;
		else
		{
			pModules->bClient = CLIENT_DLL_BASE;
			pModules->result = STATUS_SUCCESS;
		}
	}
}

void ImageLoadCallback(PUNICODE_STRING FullName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	if (wcsstr(FullName->Buffer, CLIENT_DLL))
	{
		DbgPrintEx(0, 0, "Client.dll Loaded! Base: 0x%p", ImageInfo->ImageBase);
		CLIENT_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
	}
}


//#pragma alloc_text(PAGE, DriverUnload)
//#pragma alloc_text(INIT, DriverEntry)

void DriverUnload(IN DRIVER_OBJECT pDriver)
{
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDeviceObj);

	DbgPrintEx(0, 0, "Goodbye!\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriver, IN PUNICODE_STRING RegistryPath)
{

	RtlInitUnicodeString(&DeviceName, DRIVER_NAME);
	RtlInitUnicodeString(&DosName, SYMBOL_NAME);
	pDriver->DriverUnload = DriverUnload;

	/*
	PEPROCESS pProcess;
	SIZE_T size = 0;
	
	if (NT_SUCCESS(PsLookupProcessByProcessId(process, &pProcess)))
	{
		MmCopyVirtualMemory(pProcess, (PVOID)addr, PsGetCurrentProcess(), &testVal, sizeof(DWORD32), KernelMode, &size);
		DbgPrintEx(0, 0, "Readed Value: %d", testVal);

		testVal = 1337;
		MmCopyVirtualMemory(PsGetCurrentProcess(), &testVal, pProcess, (PVOID)addr, sizeof(DWORD32), KernelMode, &size);
		DbgPrintEx(0, 0, "Written value: %d", testVal);

	}
	*/

	if (NT_SUCCESS(IoCreateDevice(pDriver, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj)))
	{
		IoCreateSymbolicLink(&DosName, &DeviceName);
		DbgPrintEx(0, 0, "Device Created!\n");

		pDriver->MajorFunction[IRP_MJ_CREATE] = CreateCall;
		pDriver->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
		pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

		pDeviceObj->Flags |= DO_DIRECT_IO;
		pDeviceObj->Flags &= ~DO_DEVICE_INITIALIZING;

		PsSetLoadImageNotifyRoutine(ImageLoadCallback);


	};

	DbgPrintEx(0, 0, "Driver Loaded!\n");
	return STATUS_SUCCESS;
}