#pragma once
#include <Windows.h>
#include "BypaPH.hpp"
#include <iostream>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>
#include <string>
#include <tchar.h>
#include <windows.h>

//#define TARGET_PROCESS 11780
//#define ADDR_INT_TO_MANIPULATE (PVOID)0xA57267D0

using namespace std;


DWORD GetProcessId(LPCSTR ProcessName) {
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}



static DWORD getModuleAddress(DWORD processID, const char* moduleName)
{
	DWORD_PTR dwModuleBaseAddress = 0;
	DWORD result = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do {
				std::string moduleNameString(moduleName);
				std::string moduleNameWideString(moduleNameString.begin(), moduleNameString.end());
				if (_tcsicmp(ModuleEntry32.szModule, moduleNameWideString.c_str()) == 0)
				{
					dwModuleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
					result = dwModuleBaseAddress;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return result;
}


void LeftClick()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	Sleep(100);

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

bool IsAltPressed() {
	return GetAsyncKeyState(VK_MENU) & 0x8000;  // VK_MENU представляет клавишу "Alt"
}

int main(int argc, char *argv[])
{
	int TARGET_PROCESS = GetProcessId("hl.exe");
	cout << TARGET_PROCESS << endl;

	uintptr_t BaseAdress = getModuleAddress(TARGET_PROCESS, "client.dll");
	cout << std::hex << BaseAdress << endl;
	std::cout << "Module Adress " << ": 0x" << std::hex << BaseAdress << std::dec << std::endl;

	uintptr_t ADDR_INT_TO_MANIPULATE;
	ADDR_INT_TO_MANIPULATE = BaseAdress + 125314;

	uintptr_t CrossHair = BaseAdress + 0x125314;

	#define ADDR_INT_TO_MANIPULATE (PVOID)CrossHair

	auto bypass = new BypaPH(TARGET_PROCESS);
	#define sds =
	int iReadFromTarget = 0;
	SIZE_T bytesRead = 0;

	// Read (safe IOCTL)
	while (true) {
		if (IsAltPressed()) {
			Sleep(15);
			iReadFromTarget = bypass->qRVM<int>(ADDR_INT_TO_MANIPULATE, &bytesRead);
			cout << iReadFromTarget;
			if (iReadFromTarget == 2) {
				LeftClick();
			}
			else {}
		}
		else {}
	}


	//cout << dec << "iReadFromTarget = " << iReadFromTarget << " | bytesRead = " << bytesRead << endl;


	




	// Write
	//int iWriteInTarget = 987654;
	//SIZE_T bytesWritten = 0;
	//const auto status = bypass->qWVM(ADDR_INT_TO_MANIPULATE, &iWriteInTarget, sizeof(iWriteInTarget), &bytesWritten);
	//cout << "qWVM returned 0x" << hex << status << endl;

	// Read again (this time with the unsafe IOCTL)
	//iReadFromTarget = bypass->qRVMu<int>(ADDR_INT_TO_MANIPULATE, &bytesRead);
	//cout << dec << "iReadFromTarget = " << iReadFromTarget << " | bytesRead = " << bytesRead << endl;

	delete bypass;
	system("pause");
	return EXIT_SUCCESS;
}