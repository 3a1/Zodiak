#include <Windows.h>
#include <SubAuth.h>
#include "KernelInterface.h"
#include <cstdint>
#include <iostream>

KernelInterface ring0;

#define dwLocalPlayer			
#define dwEntityList	
#define dwGlowObject

#define m_iTeamNum	
#define m_iHealth	0x254A428C

typedef struct GlowObjectDefinition_t {
	float r;
	float g;
	float b;
	float a;
	uint8_t unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRead;
	uint8_t unk2[10];
} GlowObjectDefinition_t;

int main()
{
	while(!ring0.Attach()) { }
	while (!ring0.GetModules()) {}

	uint16_t address = 0x254A458C;
	int playerHp;

	ring0.Read(address, &playerHp);
	std::cout << playerHp << std::endl;
	Sleep(2000);
}