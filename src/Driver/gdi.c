#include "gdi.h"

bool FrameRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr, int thickness)
{
	HBRUSH oldbrush;
	RECT r = *lprc;

	if ((r.right <= r.left) || (r.bottom <= r.top)) return false;

	oldbrush = NtGdiSelectBrush(hDC, hbr);

	NtGdiPatBlt(hDC, r.left, r.top, thickness, r.bottom - r.top, PATCOPY);
	NtGdiPatBlt(hDC, r.right - thickness, r.top, thickness, r.bottom - r.top, PATCOPY);
	NtGdiPatBlt(hDC, r.left, r.top, r.right - r.left, thickness, PATCOPY);
	NtGdiPatBlt(hDC, r.left, r.bottom - thickness, r.right - r.left, thickness, PATCOPY);

	if (oldbrush)
		NtGdiSelectBrush(hDC, oldbrush);
	return TRUE;
}

bool FillRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr)
{
	BOOL Ret;
	HBRUSH prevhbr = NULL;


	prevhbr = NtGdiSelectBrush(hDC, hbr);
	Ret = NtGdiPatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
		lprc->bottom - lprc->top, PATCOPY);

	/* Select old brush */
	if (prevhbr)
		NtGdiSelectBrush(hDC, prevhbr);

	return Ret;
}


void DrawRect(
	LONG x, LONG y, LONG w, LONG h,
	unsigned char r, unsigned char g, unsigned char b)
{
	HDC hdc = NtUserGetDCEx(0x0, 0, 1);
	if (!hdc)
		return;

	HBRUSH brush = NtGdiCreateSolidBrush(RGB(r, g, b), NULL);
	if (!brush)
	{
		NtUserReleaseDC(hdc);
		return;
	}

	RECT rect = { x, y, x + w, y + h };
	FrameRect(hdc, &rect, brush, 1); // with 1 looks better especially when there is many enemies, imo ofc 
	NtUserReleaseDC(hdc);
	NtGdiDeleteObjectApp(brush);
}

void DrawFillRect(VOID* hwnd, LONG x, LONG y, LONG w, LONG h, unsigned char r, unsigned char g, unsigned char b)
{
	if (NtUserGetForegroundWindow() != (HWND)hwnd)
	{
		return;
	}

	// HDC hdc = NtUserGetDC((HWND)hwnd);
	HDC hdc = NtUserGetDCEx(0x0, 0, 1);
	UNREFERENCED_PARAMETER(hwnd);
	if (!hdc)
		return;

	HBRUSH brush = NtGdiCreateSolidBrush(RGB(r, g, b), NULL);
	if (!brush)
		return;

	RECT rect = { x, y, x + w, y + h };
	FillRect(hdc, &rect, brush);
	NtUserReleaseDC(hdc);
	NtGdiDeleteObjectApp(brush);
}

bool GetScreenResolution(Vector2* screen_out) 
{
	HDC hdc = NtUserGetDC(NULL); // Get the device context for the entire screen
	int x = NtGdiGetDeviceCaps(hdc, HORZRES);
	int y = NtGdiGetDeviceCaps(hdc, VERTRES);
	NtUserReleaseDC(hdc);
	if (!x || !y) 
	{
		return false;
	}

	screen_out->x = (float)x;
	screen_out->y = (float)y;
	return true;
}