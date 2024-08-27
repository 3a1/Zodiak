#pragma once
#include "pch.h"

#define MM_COPY_MEMORY_VIRTUAL              0x2

NTSTATUS copy_memory(void* src_process_id, void* src_address, void* dest_process_id, void* dest_address, uintptr_t size);

NTSTATUS ReadEx(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS WriteEx(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);

inline bool read(uintptr_t Address, PVOID Buffer, SIZE_T Size)
{
    return NT_SUCCESS(ReadEx((HANDLE)gPid, (PVOID)Address, Buffer, Size));
}

inline bool write(uintptr_t Address, PVOID Buffer, SIZE_T Size)
{
    return NT_SUCCESS(WriteEx((HANDLE)gPid, (PVOID)Address, Buffer, Size));
}