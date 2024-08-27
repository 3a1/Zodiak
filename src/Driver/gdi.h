#pragma once
#include "pch.h"

/* 
	Remember that to use these functions we need to be in the win32 context ! 
*/
bool GetScreenResolution(Vector2* screen_out);
bool FrameRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr, int thickness);
bool FillRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr);
void DrawRect(LONG x, LONG y, LONG w, LONG h, unsigned char r, unsigned char g, unsigned char b);
void DrawFillRect(VOID* hwnd, LONG x, LONG y, LONG w, LONG h, unsigned char r, unsigned char g, unsigned char b);