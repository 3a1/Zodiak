#include "includes.hpp"

#include "Struct.h"
#include "math.hpp"
#include "functions.hpp"
#include "angles_aimbot.hpp"

void AnglesAimbotFunction(int maxDistance) {

	auto hwnd = FindWindowA(NULL, "Counter-Strike");
	DWORD process_id = 0;
	GetWindowThreadProcessId(hwnd, &process_id);

	uintptr_t hw = getModuleAddress(process_id, "hw.dll");
	uintptr_t client = getModuleAddress(process_id, "client.dll");

	auto driver = new driver_manager("\\\\.\\cartidriver", process_id);

	uintptr_t playerX = client + 0x13E7F0;
	uintptr_t playerY = client + 0x13E7F4;
	uintptr_t playerZ = client + 0x13E7F8;
	uintptr_t viewAnglesX = hw + 0x108AEC8;
	uintptr_t viewAnglesY = hw + 0x108AEC4;

	while (true) {

		float GdegreesX = 0;
		float GdegreesY = 0;

		for (int i = 1; i < 32; i++)
		{
			uintptr_t enemyXpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0184;

			Vector3 TargetPos;
			TargetPos.x = driver->RPM<float>(enemyXpos);
			if (TargetPos.x == 0)
			{
				continue;
			}

			Vector3 PlayerPos;
			PlayerPos.x = driver->RPM<float>(playerX);
			PlayerPos.y = driver->RPM<float>(playerY);
			PlayerPos.z = driver->RPM<float>(playerZ);

			uintptr_t enemyYpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0188;
			uintptr_t enemyZpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x018C;

			TargetPos.y = driver->RPM<float>(enemyYpos);
			TargetPos.z = driver->RPM<float>(enemyZpos);

			Vector3 viewAngle;
			viewAngle.y = driver->RPM<float>(viewAnglesY);
			viewAngle.x = driver->RPM<float>(viewAnglesX);

			float LocalPlayer[3] = { PlayerPos.x, PlayerPos.y, PlayerPos.z };
			float Target[3] = { TargetPos.x, TargetPos.y, TargetPos.z };
			float degrees[2] = { 0.0f, 0.0f };

			CalculateRawAngle(LocalPlayer, Target, degrees);

			float distanceX = std::abs(degrees[1] - viewAngle.x);
			float distanceY = std::abs(degrees[0] - viewAngle.y);

			if (distanceX > 180) {
				distanceX = 360 - distanceX;
			}
			if (distanceY > 180) {
				distanceY = 360 - distanceY;
			}

			float distance = distanceX + distanceY;

			if (distance < maxDistance) {
				GdegreesX = degrees[1];
				GdegreesY = degrees[0];
			}

		}

		if (GdegreesX == 0) {
			continue;
		}

		if (!GetAsyncKeyState(0x01) == 0) {

			//  -- Way to add smoothing
			//  -- im not using it
			/*
			float viewAngY = driver->RPM<float>(viewAnglesY);
			float viewAngX = driver->RPM<float>(viewAnglesX);
			float oldAngle[2] = { viewAngY, viewAngX };
			float degrees[2] = { GdegreesY , GdegreesX };

			float smAngleX = oldAngle[1] - degrees[1];
			float smAngleY = oldAngle[0] - degrees[0];

			float SmoothedAngleX = oldAngle[1] - smAngleX / 100;
			float SmoothedAngleY = oldAngle[0] - smAngleY / 100;
			driver->WPM<float>(viewAnglesY, SmoothedAngleY);
			driver->WPM<float>(viewAnglesX, SmoothedAngleX);
			*/

			driver->WPM<float>(viewAnglesY, GdegreesY);
			driver->WPM<float>(viewAnglesX, GdegreesX);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	}

}