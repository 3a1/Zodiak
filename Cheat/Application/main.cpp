#include "config.h"
#include "features.h"

int 
main() 
{
	ConfigRead();

	if (!isProcessAlive("hl.exe")) {
		MessageBox(nullptr, "Please run game before running cheat.", "Z3BRA", MB_OK);
		exit(0);
	}

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	std::thread aliveThread;
	std::thread matrixThread;
	std::thread updateThread;
	std::thread deadThread;
	std::thread aimbotThread;
	std::thread bhopThread;

	if (!kernel) {
		if (aimbot) {
			aliveThread = std::thread(processAlive);
			matrixThread = std::thread(UserMode::matrixUpdate);
			updateThread = std::thread(UserMode::offsetsUpdate);
			deadThread = std::thread(UserMode::deadCheck);
			aimbotThread = std::thread(UserMode::aimbotMain);
			printf("[Z3BRA] Aimbot thread started\n");
		}

		if (bhop) {
			bhopThread = std::thread(UserMode::bhopFunction);
			printf("[Z3BRA] Bhop thread started\n");
		}
	}
	else if (kernel) {
		if (aimbot) {
			aliveThread = std::thread(processAlive);
			matrixThread = std::thread(KernelMode::matrixUpdate);
			updateThread = std::thread(KernelMode::offsetsUpdate);
			deadThread = std::thread(KernelMode::deadCheck);
			aimbotThread = std::thread(KernelMode::aimbotMain);
			printf("[Z3BRA] Aimbot thread started\n");
		}

		if (bhop) {
			bhopThread = std::thread(KernelMode::bhopFunction);
			printf("[Z3BRA] Bhop thread started\n");
		}
	}

	for (;;) {}
}
