#pragma once
#include "pch.h"

NTSTATUS FindProcessByName(CHAR* process_name, PEPROCESS* process);

uintptr_t GetModuleBase64(uintptr_t pid, wchar_t* module_name);
uint32_t GetModuleBase32(uintptr_t pid, wchar_t* module_name);

QWORD GetModuleEntry(PCWSTR module_name);
QWORD GetProcAddressQ(QWORD base, PCSTR export_name);

void NtSleep(DWORD milliseconds);