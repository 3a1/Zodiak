#include "export.h"

bool exports_init()
{
	QWORD win32kbase = GetModuleEntry(L"win32kbase.sys");
	if (!win32kbase)
	{
		return false;
	}

	QWORD win32kfull = GetModuleEntry(L"win32kfull.sys");
	if (!win32kfull)
	{
		return false;
	}

	NtGdiGetDeviceCaps = (GetDeviceCaps_t)GetProcAddressQ(win32kbase, "NtGdiGetDeviceCaps");
	if (!NtGdiGetDeviceCaps)
	{
		return false;
	}

	NtUserGetAsyncKeyState = (GetAsyncKeyState_t)GetProcAddressQ(win32kbase, "NtUserGetAsyncKeyState");
	if (!NtUserGetAsyncKeyState)
	{
		return false;
	}

	NtUserGetForegroundWindow = (GetForegroundWindow_t)GetProcAddressQ(win32kfull, "NtUserGetForegroundWindow");
	if (!NtUserGetForegroundWindow)
	{
		return false;
	}

	NtUserGetDC = (GetDC_t)GetProcAddressQ(win32kbase, "NtUserGetDC");
	if (!NtUserGetDC)
	{
		return false;
	}

	NtUserGetDCEx = (GetDCEx_t)GetProcAddressQ(win32kfull, "NtUserGetDCEx");
	if (!NtUserGetDCEx)
	{
		return false;
	}

	NtGdiPatBlt = (PatBlt_t)GetProcAddressQ(win32kfull, "NtGdiPatBlt");
	if (!NtGdiPatBlt)
	{
		return false;
	}

	NtGdiSelectBrush = (SelectBrush_t)GetProcAddressQ(win32kbase, "GreSelectBrush");
	if (!NtGdiSelectBrush)
	{
		return false;
	}

	NtUserReleaseDC = (ReleaseDC_t)GetProcAddressQ(win32kbase, "NtUserReleaseDC");
	if (!NtUserReleaseDC)
	{
		return false;
	}

	NtGdiCreateSolidBrush = (CreateSolidBrush_t)GetProcAddressQ(win32kfull, "NtGdiCreateSolidBrush");
	if (!NtGdiCreateSolidBrush)
	{
		return false;
	}

	NtGdiDeleteObjectApp = (DeleteObjectApp_t)GetProcAddressQ(win32kbase, "NtGdiDeleteObjectApp");
	if (!NtGdiDeleteObjectApp)
	{
		return false;
	}

	NtGdiExtTextOutW = (ExtTextOutW_t)GetProcAddressQ(win32kfull, "NtGdiExtTextOutW");
	if (!NtGdiExtTextOutW)
	{
		return false;
	}

	NtGdiHfontCreate = (HfontCreate_t)GetProcAddressQ(win32kfull, "hfontCreate");
	if (!NtGdiHfontCreate)
	{
		return false;
	}

	NtGdiSelectFont = (SelectFont_t)GetProcAddressQ(win32kfull, "NtGdiSelectFont");
	if (!NtGdiSelectFont)
	{
		return false;
	}

	return 1;
}