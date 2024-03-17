#pragma once
#include "driver_io.h"

struct CSGoModules
{
	DWORD32 bClient;
};

class KernelInterface
{
public:
	BOOLEAN NoErrors;
	CSGoModules* Modules;
	KernelInterface();
	bool Attach();
	bool GetModules();
	~KernelInterface();



	template<typename T>
	_inline void Read(DWORD32 address, T* result)
	{
		NoErrors = false;
		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;
		
		KERNEL_READ_REQUEST req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Response = (DWORD64)result;
		req.Result = -1;

		BOOL rsp = DeviceIoControl(m_hDriver, IO_READ_PROCESS_MEMORY, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = GetLastError();
		}
		else if (!NT_SUCCESS(req.Result))
		{
			m_dwErrorCode = req.Result;
		}

		NoErrors = true;
	}

	template<typename T>
	_inline void Write(DWORD32 address, T* val)
	{
		NoErrors = false;
		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_WRITE_REQUEST req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Value = (DWORD64)val;
		req.Result = -1;

		BOOL rsp = DeviceIoControl(m_hDriver, IO_WRITE_PROCESS_MEMORY, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = GetLastError();
		}
		else if (!NT_SUCCESS(req.Result))
		{
			m_dwErrorCode = req.Result;
		}

		NoErrors = true;
	}

private:
	HANDLE m_hDriver;
	DWORD32 m_dwProcessId;
	DWORD m_dwErrorCode;
	HANDLE m_hProcess;
};