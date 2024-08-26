#pragma once
#include "pch.h"

/* Gdi stuff */
typedef HDC(*GetDC_t)(HWND hwnd);//verified
typedef HDC(*GetDCEx_t)(HWND hwnd, HANDLE region, ULONG flags);//verified
typedef BOOL(*PatBlt_t)(HDC hdcDest, INT x, INT y, INT cx, INT cy, DWORD dwRop);//verified
typedef HBRUSH(*SelectBrush_t)(HDC hdc, HBRUSH hbrush); //verified
typedef int (*ReleaseDC_t)(HDC hdc); //verified
typedef HBRUSH(*CreateSolidBrush_t)(COLORREF cr, HBRUSH hbr); //verified
typedef BOOL(*DeleteObjectApp_t)(HANDLE hobj); //verified
typedef BOOL(*ExtTextOutW_t)(IN HDC hDC, //verified
	IN INT 	XStart,
	IN INT 	YStart,
	IN UINT 	fuOptions,
	IN OPTIONAL LPRECT 	UnsafeRect,
	IN LPWSTR 	UnsafeString,
	IN INT 	Count,
	IN OPTIONAL LPINT 	UnsafeDx,
	IN DWORD 	dwCodePage
	);
typedef HFONT(*HfontCreate_t)(IN PENUMLOGFONTEXDVW pelfw, IN ULONG cjElfw, IN DWORD lft, IN FLONG fl, IN PVOID pvCliData); //verified
typedef HFONT(*SelectFont_t)(_In_ HDC 	hdc, //verified
	_In_ HFONT 	hfont
	);
typedef int (*GetDeviceCaps_t)(HDC a1, int a2);

/* Windows Functions */
typedef int (*GetAsyncKeyState_t)(INT a);
typedef HWND(*GetForegroundWindow_t)();

/* 
	Remember that to use these functions need to be in the win32 context ! 
*/

/* Windows Functions */
GetAsyncKeyState_t NtUserGetAsyncKeyState;
GetForegroundWindow_t NtUserGetForegroundWindow;

/* GDI Exports */
GetDC_t NtUserGetDC;
GetDCEx_t NtUserGetDCEx;
SelectBrush_t NtGdiSelectBrush;
PatBlt_t NtGdiPatBlt;
ReleaseDC_t NtUserReleaseDC;
CreateSolidBrush_t NtGdiCreateSolidBrush;
DeleteObjectApp_t NtGdiDeleteObjectApp;
ExtTextOutW_t NtGdiExtTextOutW;
HfontCreate_t NtGdiHfontCreate;
SelectFont_t NtGdiSelectFont;
GetDeviceCaps_t NtGdiGetDeviceCaps;

bool exports_init();