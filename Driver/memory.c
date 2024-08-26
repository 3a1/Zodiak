#include "memory.h"

NTSTATUS copy_memory(void* src_process_id, void* src_address, void* dest_process_id, void* dest_address, uintptr_t size)
{
    if (src_process_id == (void*)4ULL)
    {
        RtlCopyMemory(dest_address, src_address, size);
        return STATUS_SUCCESS;
    }
    else
    {
        void* SrcProc = 0;
        void* DstProc = 0;
        uintptr_t size_out = 0;
        NTSTATUS status = 0;

        status = PsLookupProcessByProcessId(src_process_id, &(PEPROCESS)SrcProc);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        status = PsLookupProcessByProcessId(dest_process_id, &(PEPROCESS)DstProc);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        status = MmCopyVirtualMemory(SrcProc, src_address, DstProc, dest_address, size, 1, &size_out);

        // Dereference SrcProc and DstProc to prevent memory leaks
        ObDereferenceObject(SrcProc);
        ObDereferenceObject(DstProc);

        return status;
    }
}

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
    SIZE_T Bytes;
    if (NT_SUCCESS(MmCopyVirtualMemory(Process, SourceAddress, PsGetCurrentProcess(),
        TargetAddress, Size, KernelMode, &Bytes)))
        return STATUS_SUCCESS;
    else
        return STATUS_ACCESS_DENIED;
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
    SIZE_T Bytes;
    if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), SourceAddress, Process,
        TargetAddress, Size, KernelMode, &Bytes)))
        return STATUS_SUCCESS;
    else
        return STATUS_ACCESS_DENIED;
}

NTSTATUS ReadEx(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size)
{
    PEPROCESS Process;
    NTSTATUS Status;

    if (!NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &Process))) 
    {
        return STATUS_INVALID_PARAMETER;
    }

    Status = KeReadVirtualMemory(Process, Address, Buffer, Size);

    if (NT_SUCCESS(Status)) 
    {
        return STATUS_SUCCESS;
    }
    else
    {
        return Status;
    }
}

NTSTATUS WriteEx(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size)
{
    PEPROCESS Process;
    NTSTATUS Status;

    if (!NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &Process))) 
    {
        return STATUS_INVALID_PARAMETER;
    }

    Status = KeWriteVirtualMemory(Process, Address, Buffer, Size);

    if (NT_SUCCESS(Status)) 
    {
        return STATUS_SUCCESS;
    }
    else 
    {
        return Status;
    }
}