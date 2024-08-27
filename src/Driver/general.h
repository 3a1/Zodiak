#pragma once
#include "pch.h"

/* Offsets */
#define dwViewMatrix          0xEC9780
#define dwEntityList          0x12043CC
#define dwLocalTeam           0x100DF4
#define m_entityOrigin        0x184
#define m_stateValue          0x17C
#define m_entityModel         0x12C
#define m_entitySize          0x250

/* Settings */
#define MAX_ENTITIES          64
#define AIMBOT_KEY            0x1		// VK_LBUTTON

// change these if aimbot points below or higher than player head
// or want to make bigger esp box or something
#define AIMBOT_HEAD           20.f      // Head position (relatively of the player origin)
#define ESP_BOX_BOTTOM        30.f      // Box bottom position (relatively of the player origin) 
#define ESP_BOX_TOP           25.f      // Box top position (relatively of the player origin)

#define ESP_TOGGLE_KEY        0x76		// F7
#define AIMBOT_TOGGLE_KEY     0x75		// F6

#define SMOOTH_UP_KEY         0x26		// VK_UP
#define SMOOTH_DOWN_KEY       0x28		// VK_DOWN
#define FOV_UP_KEY            0x27		// VK_RIGHT
#define FOV_DOWN_KEY          0x25		// VK_LEFT

#define ONLY_ENEMIES_KEY      0x77		// F8

static RGB box_color = { 255.0f, 0.0f, 0.0f };

static float aimbot_smooth = 3.f;
static int aimbot_fov = 100;


/* Global variables */
static CHAR* gProcessName = "hl.exe";;
PEPROCESS gProcess;
ULONG64 gPid;

/* Others */
#define DBG 1
#if DBG
#define DebugPrint(x,...) DbgPrint("[zodiak] " x, __VA_ARGS__)
#else
#define DebugPrint(x,...)
#endif

// sb: There is no useless things in programming !
// this big foot nygga:
#define __FLTUSED__
__declspec(selectany) int _fltused = 1;
// virgin nerd in the corner: "fltused is important flag for compiler to allow support float !!!" 