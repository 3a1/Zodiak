#pragma once
#include "includes.h"

bool processAlive();
bool isProcessAlive(const char* processName);
double distance(int x1, int y1, int x2, int y2);
void aimAtPos(float x, float y, int AimSpeed, int SmoothAmount);
bool stringToBool(const std::string& str);