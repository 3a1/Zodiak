#pragma once
#include <Windows.h>
#include "BypaPH.hpp"
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <tchar.h>
#include <future>
#include <thread>
#include "memory.h"
#include "vector.h"


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

bool IsAltPressed() {
	return GetAsyncKeyState(VK_MENU) & 0x8000;
}

void CalcAngleNew(float* src, float* dst, float* angles)
{
	double delta[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	angles[0] = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
	angles[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;
	if (delta[0] >= 0.0) { angles[1] += 180.0f; }
}

int main(int argc, char *argv[])
{
	int TARGET_PROCESS = GetProcessId("hl.exe");

	uintptr_t client = getModuleAddress(TARGET_PROCESS, "client.dll");
	uintptr_t hw = getModuleAddress(TARGET_PROCESS, "hw.dll");

	uintptr_t entity = hw + 0x120461C;
	uintptr_t entityX = entity + 0x0184;
	uintptr_t entityY = entity + 0x0188;
	uintptr_t entityZ = entity + 0x018C;
	uintptr_t playerX = client + 0x13E7F0;
	uintptr_t playerY = client + 0x13E7F4;
	uintptr_t playerZ = client + 0x13E7F8;
	uintptr_t viewAnglesX = hw + 0x108AEC8;
	uintptr_t viewAnglesY = hw + 0x108AEC4;

	auto bypass = new BypaPH(TARGET_PROCESS);

	while (true) {

		int iReadFromTarget = 0;
		SIZE_T bytesRead = 0;
		SIZE_T bytesWritten = 0;

		float PunchY1;
		float PunchX1;

		float minDistance = 999999;
		int closestPlayer = 0;

		for (int i = 1; i < 32; i++) {
			uintptr_t enemyX = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0184;
			uintptr_t enemyY = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0188;

			Vector3 PlayerPos;
			PlayerPos.x = bypass->qRVM<float>((PVOID)playerX, &bytesRead);
			PlayerPos.y = bypass->qRVM<float>((PVOID)playerY, &bytesRead);
			PlayerPos.z = bypass->qRVM<float>((PVOID)playerZ, &bytesRead);

			uintptr_t enemyXpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0184;
			uintptr_t enemyYpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0188;
			uintptr_t enemyZpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x018C;

			Vector3 TargetPos;
			TargetPos.x = bypass->qRVM<float>((PVOID)enemyXpos, &bytesRead);
			if (TargetPos.x == 0) {
				continue;
			}
			TargetPos.y = bypass->qRVM<float>((PVOID)enemyYpos, &bytesRead);
			TargetPos.z = bypass->qRVM<float>((PVOID)enemyZpos, &bytesRead);

			Vector3 viewAngle;
			viewAngle.y = bypass->qRVM<float>((PVOID)viewAnglesY, &bytesRead);
			viewAngle.x = bypass->qRVM<float>((PVOID)viewAnglesX, &bytesRead);

			float LocalPlayer[3] = { PlayerPos.x, PlayerPos.y, PlayerPos.z };
			float Target[3] = { TargetPos.x, TargetPos.y, TargetPos.z };
			float angles[2] = { 0.0f, 0.0f };

			CalcAngleNew(LocalPlayer, Target, angles);

			float PunchY = angles[0] - viewAngle.y;
			float PunchX = angles[1] - viewAngle.x;

			//float distanceX = std::abs(PunchX);
			//float distanceY = std::abs(PunchY);

			float distanceX = std::abs(angles[1] - viewAngle.x);
			float distanceY = std::abs(angles[0] - viewAngle.y);
			if (distanceX > 180) {
				distanceX = 360 - distanceX;
			}
			if (distanceY > 180) {
				distanceY = 360 - distanceY;
			}

			//float distance = sqrt(pow(PlayerPos.x - TargetPos.x, 2) + pow(PlayerPos.y - TargetPos.y, 2) + pow(PlayerPos.z - TargetPos.z, 2));

			//float distance = sqrt(pow(viewAngle.y - angles[0], 2) + pow(viewAngle.x - angles[1], 2));
			float distance = distanceX + distanceY;

			//if (minDistance > distance) {
				//minDistance = distance;
				//closestPlayer = i;
			//}

			if (distance < 4) {
				closestPlayer = i;
			}
		}

		if (IsAltPressed()) {

			if (closestPlayer == 0) {
				continue;
			}

			Vector3 PlayerPos;
			PlayerPos.x = bypass->qRVM<float>((PVOID)playerX, &bytesRead);
			PlayerPos.y = bypass->qRVM<float>((PVOID)playerY, &bytesRead);
			PlayerPos.z = bypass->qRVM<float>((PVOID)playerZ, &bytesRead);

			uintptr_t enemyXpos = hw + 0x120461C - 0x0250 + closestPlayer * 0x0250 + 0x0184;
			uintptr_t enemyYpos = hw + 0x120461C - 0x0250 + closestPlayer * 0x0250 + 0x0188;
			uintptr_t enemyZpos = hw + 0x120461C - 0x0250 + closestPlayer * 0x0250 + 0x018C;

			Vector3 TargetPos;
			TargetPos.x = bypass->qRVM<float>((PVOID)enemyXpos, &bytesRead);
			TargetPos.y = bypass->qRVM<float>((PVOID)enemyYpos, &bytesRead);
			TargetPos.z = bypass->qRVM<float>((PVOID)enemyZpos, &bytesRead);

			Vector3 viewAngle;
			viewAngle.y = bypass->qRVM<float>((PVOID)viewAnglesY, &bytesRead);
			viewAngle.x = bypass->qRVM<float>((PVOID)viewAnglesX, &bytesRead);

			float LocalPlayer[3] = { PlayerPos.x, PlayerPos.y, PlayerPos.z };
			float Target[3] = { TargetPos.x, TargetPos.y, TargetPos.z };
			float angles[2] = { 0.0f, 0.0f };

			CalcAngleNew(LocalPlayer, Target, angles);

			float PunchY = viewAngle.y - angles[0];
			float PunchX = viewAngle.x - angles[1];

			float PunchY1 = viewAngle.y - PunchY - 12.f;
			float PunchX1 = viewAngle.x - PunchX;



			auto A = bypass->qWVM((PVOID)viewAnglesY, &PunchY1, sizeof(angles[0]), &bytesWritten);
			auto B = bypass->qWVM((PVOID)viewAnglesX, &PunchX1, sizeof(angles[1]), &bytesWritten);
		}

	}


delete bypass;
system("pause");
return EXIT_SUCCESS;
}
