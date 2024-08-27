#include "utils.h"

__declspec(dllimport) LIST_ENTRY* PsLoadedModuleList;

NTSTATUS FindProcessByName(CHAR* process_name, PEPROCESS* process)
{
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;

	CHAR image_name[15];

	do
	{
		RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + 0x5a8) /*EPROCESS->ImageFileName*/, sizeof(image_name));

		if (strstr(image_name, process_name))
		{
			DWORD active_threads;
			RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + 0x5f0) /*EPROCESS->ActiveThreads*/, sizeof(active_threads));
			if (active_threads)
			{
				*process = cur_entry;
				return STATUS_SUCCESS;
			}
		}

		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x448) /*EPROCESS->ActiveProcessLinks*/;
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x448);

	} while (cur_entry != sys_process);

	return STATUS_NOT_FOUND;
}


uintptr_t GetModuleBase64(uintptr_t pid, wchar_t* module_name)
{
	void* ProcessPtr = 0;

	// Find process by ID
	if (PsLookupProcessByProcessId((HANDLE)pid, &(PEPROCESS)ProcessPtr) < 0 || ProcessPtr == 0)
	{
		return 0;
	}

	PPEB pPeb = (PPEB)PsGetProcessPeb(ProcessPtr); // Return Base Address
	if (!pPeb)
	{
		ObDereferenceObject(ProcessPtr); // Clean up process object
		return 0;
	}

	KAPC_STATE state;
	KeStackAttachProcess(ProcessPtr, &state);

    PPEB_LDR_DATA pLdr = (PPEB_LDR_DATA)pPeb->Ldr;
	if (!pLdr)
	{
		KeUnstackDetachProcess(&state);
		ObDereferenceObject(ProcessPtr); // Clean up process object
		return 0;
	}

	// Assuming module_name is a wchar_t* (null-terminated wide string)
	uintptr_t address = 0;

	for (PLIST_ENTRY list = (PLIST_ENTRY)pLdr->InMemoryOrderModuleList.Flink;
		list != &pLdr->InMemoryOrderModuleList; list = (PLIST_ENTRY)list->Flink)
	{
		PLDR_DATA_TABLE_ENTRY pEntry =
			CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		// Output the FullDllName using DbgPrint
		//DbgPrint("Checking module: %wZ\n", &pEntry->FullDllName);

		// Check if module_name is a substring of pEntry->FullDllName.Buffer
		if (wcsstr(pEntry->FullDllName.Buffer, module_name) != NULL)
		{
			ULONG64 baseAddr = (ULONG64)pEntry->DllBase;
			address = baseAddr;
			break;  // Exit loop once the module is found
		}
	}

	KeUnstackDetachProcess(&state);
	ObDereferenceObject(ProcessPtr); // Clean up process object

	return address;
}

uint32_t GetModuleBase32(uintptr_t pid, wchar_t* module_name)
{
	void* ProcessPtr = 0;

	// Find process by ID
	if (PsLookupProcessByProcessId((HANDLE)pid, &(PEPROCESS)ProcessPtr) < 0 || ProcessPtr == 0)
	{
		return 0;
	}

	PPEB32 pPeb = (PPEB32)PsGetProcessWow64Process(ProcessPtr);
	if (!pPeb)
	{
		ObDereferenceObject(ProcessPtr); // Clean up process object
		return 0;
	}

	KAPC_STATE state;
	KeStackAttachProcess(ProcessPtr, &state);

	PPEB_LDR_DATA32 pLdr = (PPEB_LDR_DATA32)pPeb->Ldr;
	if (!pLdr)
	{
		KeUnstackDetachProcess(&state);
		ObDereferenceObject(ProcessPtr); // Clean up process object
		return 0;
	}

	// Assuming module_name is a wchar_t* (null-terminated wide string)
	uint32_t address = 0;

	for (PLIST_ENTRY32 list = (PLIST_ENTRY32)pLdr->InMemoryOrderModuleList.Flink;
		list != &pLdr->InMemoryOrderModuleList; list = (PLIST_ENTRY32)list->Flink)
	{
		PLDR_DATA_TABLE_ENTRY32 pEntry =
			CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY32, InMemoryOrderLinks);

		// Output the FullDllName using DbgPrint
		//DbgPrint("Checking module: %wZ\n", (const wchar_t*)&pEntry->FullDllName);

		// Check if module_name is a substring of pEntry->FullDllName.Buffer
		if (wcsstr((const wchar_t*)pEntry->FullDllName.Buffer, module_name) != NULL)
		{
			ULONG32 baseAddr = (ULONG32)pEntry->DllBase;
			address = baseAddr;
			break;  // Exit loop once the module is found
		}
	}

	KeUnstackDetachProcess(&state);
	ObDereferenceObject(ProcessPtr); // Clean up process object

	return address;
}

QWORD GetModuleEntry(PCWSTR module_name)
{
	for (PLIST_ENTRY pListEntry = PsLoadedModuleList->Flink; pListEntry != PsLoadedModuleList; pListEntry = pListEntry->Flink)
	{
		_PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (pEntry->ImageBase == 0)
			continue;

		if (pEntry->BaseImageName.Buffer && !wcscmp(pEntry->BaseImageName.Buffer, module_name))
		{
			return (QWORD)pEntry->ImageBase;
		}

	}
	return 0;
}

QWORD GetProcAddressQ(QWORD base, PCSTR export_name)
{
	QWORD a0;
	DWORD a1[4];


	a0 = base + *(USHORT*)(base + 0x3C);
	a0 = base + *(DWORD*)(a0 + 0x88);
	a1[0] = *(DWORD*)(a0 + 0x18);
	a1[1] = *(DWORD*)(a0 + 0x1C);
	a1[2] = *(DWORD*)(a0 + 0x20);
	a1[3] = *(DWORD*)(a0 + 0x24);
	while (a1[0]--) {

		a0 = base + *(DWORD*)(base + a1[2] + (a1[0] * 4));
		if (strcmp((PCSTR)a0, export_name) == 0) {

			return (base + *(DWORD*)(base + a1[1] + (*(USHORT*)(base + a1[3] + (a1[0] * 2)) * 4)));
		}
	}
	return 0;
}

/* very clean func, thx ekknod :3 */
void NtSleep(DWORD milliseconds)
{
	QWORD ms = milliseconds;
	ms = (ms * 1000) * 10;
	ms = ms * -1;
#ifdef _KERNEL_MODE
	KeDelayExecutionThread(KernelMode, 0, (PLARGE_INTEGER)&ms);
#else
	NtDelayExecution(0, (PLARGE_INTEGER)&ms);
#endif
}