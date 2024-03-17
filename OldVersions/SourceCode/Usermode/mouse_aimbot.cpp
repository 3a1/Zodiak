#include "includes.hpp"

#include "Struct.h"
#include "math.hpp"
#include "functions.hpp"
#include "mouse_aimbot.hpp"



void MouseAimbotFunction(int screenWidth, int screenHeight, int maxDistance) {

	auto hwnd = FindWindowA(NULL, "Counter-Strike");
	DWORD process_id = 0;
	GetWindowThreadProcessId(hwnd, &process_id);

	uintptr_t hw = getModuleAddress(process_id, "hw.dll");
	uintptr_t client = getModuleAddress(process_id, "client.dll");

	auto driver = new driver_manager("\\\\.\\cartidriver", process_id);

	uintptr_t entity = hw + 0x120461C;
	uintptr_t entityX = entity + 0x0184;
	uintptr_t entityY = entity + 0x0188;
	uintptr_t entityZ = entity + 0x018C;
	uintptr_t playerX = client + 0x13E7F0;
	uintptr_t playerY = client + 0x13E7F4;
	uintptr_t playerZ = client + 0x13E7F8;
	uintptr_t viewAnglesX = hw + 0x108AEC8;
	uintptr_t viewAnglesY = hw + 0x108AEC4;

	while (true) {

		int GdeltaX = 0;
		int GdeltaY = 0;

		for (int i = 1; i < 32; i++)
		{
			uintptr_t enemyXpos = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x0184;

			Vector3 TargetPos;
			TargetPos.x = driver->RPM<float>(enemyXpos);
			if (TargetPos.x == 0)
			{
				continue;
			}

			// -- if you want check for alive(entity)
			// -- u need doing somthing like this:
			/*
			uintptr_t dwAnimState = hw + 0x120461C - 0x0250 + i * 0x0250 + 0x017C;

			float TargetAliveOld = driver->RPM<float>(dwAnimState);
			std::this_thread::sleep_for(std::chrono::nanoseconds(10000));
			float TargetAlive = driver->RPM<float>(dwAnimState);

			if (TargetAliveOld != TargetAlive)
			{
				continue;
			}
			*/
			// -- dwAnimState is a state of player animation
			// -- if player is alive, its always changing
			// -- if player is dead, value is freezes
			// -- CS 1.6 dont have normal Alive value
			// -- So we are forced to use this

			POINT cursorPos;

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

			int pixels[2] = { 0, 0 };

			degreesToPixels(screenWidth, screenHeight, viewAngle.x, viewAngle.y, degrees[1], degrees[0], pixels);

			GetCursorPos(&cursorPos);

			int maxMoveDistance = 5; // max one interaction move

			// calculate difference from cursor position and enemy position
			int deltaX = pixels[0] - cursorPos.x;
			int deltaY = pixels[1] - cursorPos.y;

			deltaX = -deltaX;
			deltaY = -deltaY;

			// calculate total distance between
			double totalDistance = sqrt(deltaX * deltaX + deltaY * deltaY);

			// there i add some not really smooth but 
			// limitation on maximum movement per cycle,
			// this makes the aimbot more obedient
			if (totalDistance > maxMoveDistance) {
				double factor = maxMoveDistance / totalDistance;
				deltaX = static_cast<int>(deltaX * factor);
				deltaY = static_cast<int>(deltaY * factor);
			}

			if (totalDistance < maxDistance) {
				GdeltaX = deltaX;
				GdeltaY = deltaY;
			}

		}

		if (GdeltaX == 0) {
			continue;
		}

		if (!GetAsyncKeyState(0x01) == 0)
		{
			mouse_event(MOUSEEVENTF_MOVE, GdeltaX, GdeltaY, 0, 0);

			// sleep made for smooth but also for stable.
			// Aimbot without sleep will be very fast and
			// shake
			std::this_thread::sleep_for(std::chrono::nanoseconds(10000));
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}