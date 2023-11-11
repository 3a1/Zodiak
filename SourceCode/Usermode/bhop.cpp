#include "includes.hpp"

#include "functions.hpp"

#define SPACEBAR GetAsyncKeyState(VK_SPACE) 
#define PRESSED 0x8000

// -- To turn off bunnyhop set to false
bool bunnyhopEnabled = true;

void BhopFunction() {

	auto hwnd = FindWindowA(NULL, "Counter-Strike");
	DWORD process_id = 0;
	GetWindowThreadProcessId(hwnd, &process_id);

	uintptr_t client = getModuleAddress(process_id, "client.dll");
	uintptr_t hw = getModuleAddress(process_id, "hw.dll");

	auto driver = new driver_manager("\\\\.\\cartidriver", process_id);

	uintptr_t dwOnGround = hw + 0x122E2D4;
	uintptr_t dwForceJump = client + 0x131434;

	int onGround = driver->RPM<int>(dwOnGround);

	while (true) {

		if (bunnyhopEnabled)
		{
			if (SPACEBAR & PRESSED) { //if spacebar is pressed
				int onGround = driver->RPM<int>(dwOnGround);
				if (onGround == 1) { //if we are on ground
					driver->WPM<int>(dwForceJump, 5); // jump
				}
				else {
					driver->WPM<int>(dwForceJump, 4); // not jump
				}
			}
		}
		// sleep for decrease cpu usage
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}