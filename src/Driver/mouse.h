#pragma once
#include "pch.h"

QWORD _KeAcquireSpinLockAtDpcLevel;
QWORD _KeReleaseSpinLockFromDpcLevel;
QWORD _IofCompleteRequest;
QWORD _IoReleaseRemoveLockEx;

BOOL mouse_open(void);
void mouse_move(long x, long y, unsigned short button_flags);