#pragma once

#define DRIVER_NAME		"\\\\.\\top_ring0"
#define GAME_NAME		"hl.exe"
#define CLIENT_DLL		"\\cstrike\\bin\\client.dll"

#define IO_INIT_CHEAT_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_MODULES					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

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


