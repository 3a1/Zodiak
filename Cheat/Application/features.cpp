#include "features.h"
#include "driver.hpp"

PlayerPosition players[64];

const auto memory = Memory{ "hl.exe" };

const auto hw = memory.getModuleAddress("hw.dll");
const auto client = memory.getModuleAddress("client.dll");

size_t viewMatrixSize = 0x40;
void* viewMatrixBuffer = malloc(viewMatrixSize);

size_t entityListSize = 0x940C;
void* entityListBuffer = malloc(entityListSize);

float gWorldToScreen[16];
int width = 0;
int height = 0;

int id = memory.getId();

auto driver = new driver_manager("\\\\.\\cartidriver", id);


namespace UserMode 
{
	void
		matrixUpdate()
	{
		while (aimbot) {

			memory.readHugeMemory(hw + 0xEC9780, viewMatrixBuffer, viewMatrixSize);
			memcpy(gWorldToScreen, viewMatrixBuffer, sizeof(gWorldToScreen));
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void
		offsetsUpdate()
	{
		float screenPositionTemp[2];
		float stateTemp = 9999;

		while (aimbot) {

			memory.readHugeMemory(hw + 0x120461C - 0x0250, entityListBuffer, entityListSize);
			for (int i = 1; i < 64; i++)
			{

				float playerX = memory.readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0184);

				if (!playerX) continue;

				float playerY = memory.readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0188);
				float playerZ = memory.readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x018C);

				Vector3 TargetPos = { playerX , playerY , playerZ };

				WorldToScreen(TargetPos, screenPositionTemp);

				if (screenPositionTemp[0] <= 0 || screenPositionTemp[1] <= 0) continue;

				if (only_enemies) {
					uintptr_t modelAddress = i * 0x0250 + 0x012C;
					std::string model;
					char ch2;

					do {
						ch2 = memory.readModuleBuffer<char>(entityListBuffer, modelAddress);
						model.push_back(ch2);
						++modelAddress;
					} while (ch2 != '\0');


					int team = memory.read<int>(client + 0x100DF4);

					if ((team == 2 && (model.find("urban") != std::string::npos ||
						model.find("gign") != std::string::npos ||
						model.find("gsg9") != std::string::npos ||
						model.find("sas") != std::string::npos)) ||
						(team == 1 && (model.find("terror") != std::string::npos ||
							model.find("leet") != std::string::npos ||
							model.find("arctic") != std::string::npos ||
							model.find("guerilla") != std::string::npos))) {
						players[i] = { {0, 0}, 0 };
						continue;
					}

				}

				players[i].screenPosition[0] = screenPositionTemp[0];
				players[i].screenPosition[1] = screenPositionTemp[1];

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}

	void
		deadCheck()
	{
		float stateTemp = 9999;
		float playerX;

		while (aimbot) {

			for (int i = 1; i < 64; i++)
			{
				playerX = memory.readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0184);

				if (!playerX) continue;

				stateTemp = memory.readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x017C + 0x1);

				std::this_thread::sleep_for(std::chrono::milliseconds(10));

				if (stateTemp == players[i].state) {
					if (!players[i].dead) {
						players[i].screenPosition[0] = 0;
						players[i].screenPosition[1] = 0;
						players[i].dead = true;
					}
					players[i].state = stateTemp;
					continue;
				}
				else {
					players[i].dead = false;
					players[i].state = stateTemp;
				}

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}

	void
		aimbotMain()
	{
		while (aimbot) {
			if (GetAsyncKeyState(VK_LBUTTON)) {

				int centerX = width / 2;
				int centerY = height / 2;
				int fovHalf = fov / 2;
				int lowerX = centerX - fovHalf;
				int upperX = centerX + fovHalf;
				int lowerY = centerY - fovHalf;
				int upperY = centerY + fovHalf;

				double closestDistance = 999999999.0;
				int closestX = 0;
				int closestY = 0;

				for (int i = 1; i < 64; i++)
				{
					int x = players[i].screenPosition[0];
					int y = players[i].screenPosition[1];

					if (x <= 0 || y <= 0 || players[i].dead) continue;

					if (x > lowerX && x < upperX && y > lowerY && y < upperY) {
						double dist = distance(centerX, centerY, x, y);
						if (dist < closestDistance) {
							closestDistance = dist;
							closestX = x;
							closestY = y;
						}
					}
				}

				if (closestDistance < 999999999.0) {
					aimAtPos(closestX, closestY, speed, smooth);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

	void
		bhopFunction()
	{
		uintptr_t dwOnGround = hw + 0x122E2D4;
		uintptr_t dwForceJump = client + 0x131434;

		while (bhop)
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				int onGround = memory.read<int>(dwOnGround);
				if (onGround == 1)
				{
					memory.write<int>(dwForceJump, 5);
				}
				else
				{
					memory.write<int>(dwForceJump, 4);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

namespace KernelMode
{
	void
		matrixUpdate()
	{
		while (aimbot) {
			driver->readHugeMemory(hw + 0xEC9780, viewMatrixBuffer, viewMatrixSize);
			memcpy(gWorldToScreen, viewMatrixBuffer, sizeof(gWorldToScreen));
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void
		offsetsUpdate()
	{
		float screenPositionTemp[2];
		float stateTemp = 9999;

		while (aimbot) {

			driver->readHugeMemory(hw + 0x120461C - 0x0250, entityListBuffer, entityListSize);
			for (int i = 1; i < 64; i++)
			{

				float playerX = driver->readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0184);

				if (!playerX) continue;

				float playerY = driver->readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0188);
				float playerZ = driver->readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x018C);

				Vector3 TargetPos = { playerX , playerY , playerZ };

				WorldToScreen(TargetPos, screenPositionTemp);

				if (screenPositionTemp[0] <= 0 || screenPositionTemp[1] <= 0) continue;

				if (only_enemies) {
					uintptr_t modelAddress = i * 0x0250 + 0x012C;
					std::string model;
					char ch2;

					do {
						ch2 = driver->readModuleBuffer<char>(entityListBuffer, modelAddress);
						model.push_back(ch2);
						++modelAddress;
					} while (ch2 != '\0');


					int team = driver->RPM<int>(client + 0x100DF4);

					if ((team == 2 && (model.find("urban") != std::string::npos ||
						model.find("gign") != std::string::npos ||
						model.find("gsg9") != std::string::npos ||
						model.find("sas") != std::string::npos)) ||
						(team == 1 && (model.find("terror") != std::string::npos ||
							model.find("leet") != std::string::npos ||
							model.find("arctic") != std::string::npos ||
							model.find("guerilla") != std::string::npos))) {
						players[i] = { {0, 0}, 0 };
						continue;
					}

				}

				players[i].screenPosition[0] = screenPositionTemp[0];
				players[i].screenPosition[1] = screenPositionTemp[1];

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}

	void
		deadCheck()
	{
		float stateTemp = 9999;
		float playerX;

		while (aimbot) {

			for (int i = 1; i < 64; i++)
			{
				playerX = driver->readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x0184);

				if (!playerX) continue;

				stateTemp = driver->readModuleBuffer<float>(entityListBuffer, i * 0x0250 + 0x017C + 0x1);

				std::this_thread::sleep_for(std::chrono::milliseconds(10));

				if (stateTemp == players[i].state) {
					if (!players[i].dead) {
						players[i].screenPosition[0] = 0;
						players[i].screenPosition[1] = 0;
						players[i].dead = true;
					}
					players[i].state = stateTemp;
					continue;
				}
				else {
					players[i].dead = false;
					players[i].state = stateTemp;
				}

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}

	void
		aimbotMain()
	{
		while (aimbot) {
			if (GetAsyncKeyState(VK_LBUTTON)) {

				int centerX = width / 2;
				int centerY = height / 2;
				int fovHalf = fov / 2;
				int lowerX = centerX - fovHalf;
				int upperX = centerX + fovHalf;
				int lowerY = centerY - fovHalf;
				int upperY = centerY + fovHalf;

				double closestDistance = 999999999.0;
				int closestX = 0;
				int closestY = 0;

				for (int i = 1; i < 64; i++)
				{
					int x = players[i].screenPosition[0];
					int y = players[i].screenPosition[1];

					if (x <= 0 || y <= 0 || players[i].dead) continue;

					if (x > lowerX && x < upperX && y > lowerY && y < upperY) {
						double dist = distance(centerX, centerY, x, y);
						if (dist < closestDistance) {
							closestDistance = dist;
							closestX = x;
							closestY = y;
						}
					}
				}

				if (closestDistance < 999999999.0) {
					aimAtPos(closestX, closestY, speed, smooth);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

	void
		bhopFunction()
	{
		uintptr_t dwOnGround = hw + 0x122E2D4;
		uintptr_t dwForceJump = client + 0x131434;

		while (bhop)
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				int onGround = driver->RPM<int>(dwOnGround);
				if (onGround == 1)
				{
					driver->WPM<int>(dwForceJump, 5);
				}
				else
				{
					driver->WPM<int>(dwForceJump, 4);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}