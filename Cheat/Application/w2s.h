#pragma once
#include "includes.h"
#include "struct.h"
#include "features.h"

bool 
	ScreenTransform(Vector3 vPoint, float* vScreen);
bool 
	WorldToScreen(Vector3 vPoint, float* vScreen);