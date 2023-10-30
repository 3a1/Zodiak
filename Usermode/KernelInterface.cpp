#include <Windows.h>
#include <SubAuth.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "KernelInterface.h"

KernelInterface::KernelInterface()
{
	m_hDriver = CreateFile(
		DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		0,
		0);
	m_dwErrorCode = 0;
	m_dwProcessId = 0;
	m_hProcess = INVALID_HANDLE_VALUE;
	Modules = new CSGoModules();
	if (m_hDriver == INVALID_HANDLE_VALUE)
	{
		m_dwErrorCode = GetLastError();
	}
	NoErrors = true;
}

bool KernelInterface::Attach()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry = { };
	entry.dwSize = sizeof(entry);
	do
		if (!strcmp(_bstr_t(entry.szExeFile), GAME_NAME))
		{
			m_dwProcessId = entry.th32ProcessID;
			m_hProcess = OpenProcess(SYNCHRONIZE, false, m_dwProcessId);
			CloseHandle(hSnapshot);
			break;
		}
	while (Process32Next(hSnapshot, &entry));
	if (m_dwProcessId == 0)
		return false;

	KERNEL_INIT_DATA_REQUEST req{ m_dwProcessId, GetCurrentProcessId(), -1 };
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;

	BOOL rsp = DeviceIoControl(m_hDriver, IO_INIT_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if (!rsp)
	{
		m_dwErrorCode = GetLastError();
		return false;
	}

	if (!NT_SUCCESS(req.result))
	{
		m_dwErrorCode = req.result;
		return false;
	}

	NoErrors = true;
	return false;
}

bool KernelInterface::GetModules()
{
	NoErrors = false;
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;

	KERNEL_GET_MODULES_REQUEST req{ 0, -1 };

	BOOL result = DeviceIoControl(m_hDriver, IO_GET_MODULES, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if (!result)
		return false;

	if (!NT_SUCCESS(req.result))
		return false;

	Modules->bClient = req.bClient;
	return true;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
	CloseHandle(m_hProcess);
	delete Modules;
}