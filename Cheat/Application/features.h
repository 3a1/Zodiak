#pragma once
#include "includes.h"
#include "memory.h"
#include "struct.h"
#include "w2s.h"
#include "functions.h"
#include "config.h"

extern float gWorldToScreen[16];
extern int width;
extern int height;

extern int id;

namespace UserMode
{
	void
		matrixUpdate();

	void
		offsetsUpdate();

	void
		deadCheck();

	void
		aimbotMain();

	void
		bhopFunction();
}

namespace KernelMode
{
	void
		matrixUpdate();

	void
		offsetsUpdate();

	void
		deadCheck();

	void
		aimbotMain();

	void
		bhopFunction();
}