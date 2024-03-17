#pragma once

#include <Windows.h>

void CalculateRawAngle(float* src, float* dst, float* degrees);

double degreesToRadians(double degrees);

void degreesToPixels(int screenWidth, int screenHeight, float currentX, float currentY, float targetX, float targetY, int* pixel);