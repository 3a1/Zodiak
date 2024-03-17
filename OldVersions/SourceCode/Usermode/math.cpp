#include "driver.hpp"
#include <Tlhelp32.h>
#include <thread>
#include <string>
#include "iostream"

#include "Struct.h"

#define M_PI 3.14159265358979323846

void CalculateRawAngle(float* src, float* dst, float* degrees)
{
	double delta[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };

	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	degrees[0] = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
	degrees[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	degrees[2] = 0.0f;

	if (delta[0] >= 0.0) { degrees[1] += 180.0f; }
}

double degreesToRadians(double degrees) {
	return degrees * M_PI / 180.0;
}

void degreesToPixels(int screenWidth, int screenHeight, float currentX, float currentY, float targetX, float targetY, int* pixel) {
	// correct degrees
	if (currentX > 180) {
		currentX -= 360;
	}
	if (targetX > 180) {
		targetX -= 360;
	}

	// Calculate difference
	float deltaX = targetX - currentX;
	float deltaY = targetY - currentY;

	// Calculate radians to pixels on screen
	pixel[0] = (screenWidth / 2) + (screenWidth / 360) * deltaX;
	pixel[1] = (screenHeight / 2) - (screenHeight / 180) * deltaY;
}