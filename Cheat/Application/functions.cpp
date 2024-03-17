#include "functions.h"

void 
aimAtPos(float x, float y, int AimSpeed, int SmoothAmount)
{
    float TargetX = 0;
    float TargetY = 0;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int ScreenCenterX = screenWidth / 2;
    int ScreenCenterY = screenHeight / 2;

    if (AimSpeed == 0) AimSpeed = 1;
    if (SmoothAmount == 0) SmoothAmount = 1;

    if (x != 0)
    {
        if (x > ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - x);
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
        }

        if (x < ScreenCenterX)
        {
            TargetX = x - ScreenCenterX;
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX < 0) TargetX = 0;
        }
    }

    if (y != 0)
    {
        if (y > ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - y);
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
        }

        if (y < ScreenCenterY)
        {
            TargetY = y - ScreenCenterY;
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY < 0) TargetY = 0;
        }
    }
    {
        TargetX /= SmoothAmount;
        TargetY /= SmoothAmount;
        if (abs(TargetX) < 1)
        {
            if (TargetX > 0)
            {
                TargetX = 1;
            }
            if (TargetX < 0)
            {
                TargetX = -1;
            }
        }
        if (abs(TargetY) < 1)
        {
            if (TargetY > 0)
            {
                TargetY = 1;
            }
            if (TargetY < 0)
            {
                TargetY = -1;
            }
        }
        mouse_event(0x0001, (UINT)(TargetX), (UINT)(TargetY), NULL, NULL);
        return;
    }
}

double 
distance(int x1, int y1, int x2, int y2) 
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

bool
stringToBool(const std::string& str)
{
    return str == "true";
}

bool 
isProcessAlive(const char* processName)
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hProcessSnap);
                return true;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return false;
}

bool
processAlive() 
{
    while (1) {
        if (!isProcessAlive("hl.exe")) {
            MessageBox(nullptr, "Game is closed. Please restart cheat.", "Z3BRA", MB_OK);
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}