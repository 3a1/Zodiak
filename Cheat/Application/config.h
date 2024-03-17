#pragma once
#include "includes.h"
#include "functions.h"

extern bool only_enemies;
extern bool aimbot;
extern bool kernel;
extern int fov;
extern int speed;
extern int smooth;
extern bool bhop;

std::string 
			config(std::string param);
void 
	ConfigRead();