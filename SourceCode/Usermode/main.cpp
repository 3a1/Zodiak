#include "includes.hpp"

#include "Struct.h"
#include "math.hpp"
#include "functions.hpp"
#include "bhop.hpp"
#include "mouse_aimbot.hpp"
#include "angles_aimbot.hpp"

int main() {

	int choose;

	std::cout << " 1. Mouse Movement Aimbot[R] [more secure] [smooth] [recommended]" << std::endl;
	std::cout << " 2. Angles Aimbot[R&W] [not stable]" << std::endl;

	std::cin >> choose;

	if (choose == 1) {
		int screenWidth;
		int screenHeight;
		int maxDistance;

		RECT desktopRect;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktopRect);

		int Width = desktopRect.right;
		int Height = desktopRect.bottom;

		std::cout << "Your Monitor Resolution: " << Width << "x" << Height << std::endl;

		HWND consoleWindow = GetConsoleWindow();

		std::cout << "Enter your ingame width resolution: ";

		std::cin >> screenWidth;

		std::cout << "Enter your ingame height resolution: ";

		std::cin >> screenHeight;

		std::cout << "Set Aimbot max distance(pov): ";

		std::cin >> maxDistance;

		std::cout << "Hiding console... " << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(4));

		ShowWindow(consoleWindow, SW_HIDE);

		std::thread bhopThread(BhopFunction);

		
		std::thread mouseAimbotThread([screenWidth, screenHeight, maxDistance]() {
			MouseAimbotFunction(screenWidth, screenHeight, maxDistance);
			});
		mouseAimbotThread.join();

		bhopThread.join();
	}
	
	else if(choose == 2) {
		int maxDistance;
		HWND consoleWindow = GetConsoleWindow();
		std::cout << "Set Aimbot max distance(pov): ";
		std::cin >> maxDistance;

		std::cout << "Hiding console... " << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(4));

		ShowWindow(consoleWindow, SW_HIDE);

		std::thread angleAimbotThread([maxDistance]() {
			AnglesAimbotFunction(maxDistance);
			});

		angleAimbotThread.join();
	}

	else {
		std::cout << "Wrong Input, Exiting...";
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	return 0;
}