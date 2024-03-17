#include "w2s.h"

bool
ScreenTransform(Vector3 vPoint, float* vScreen)
{
	vScreen[0] = gWorldToScreen[0] * vPoint.x + gWorldToScreen[4] * vPoint.y + gWorldToScreen[8] * vPoint.z + gWorldToScreen[12];
	vScreen[1] = gWorldToScreen[1] * vPoint.x + gWorldToScreen[5] * vPoint.y + gWorldToScreen[9] * vPoint.z + gWorldToScreen[13];
	float z = gWorldToScreen[3] * vPoint.x + gWorldToScreen[7] * vPoint.y + gWorldToScreen[11] * vPoint.z + gWorldToScreen[15];

	if (z == 0.0f)
	{
		return 0.0f >= z;
	}
	else
	{
		vScreen[0] = vScreen[0] * (1.0f / z);
		vScreen[1] = vScreen[1] * (1.0f / z);
		return 0.0 >= (1.0f / z);
	}
}

bool
WorldToScreen(Vector3 vPoint, float* vScreen)
{
	bool iResult = ScreenTransform(vPoint, vScreen);
	if (vScreen[0] < 1 && vScreen[1] < 1 && vScreen[0] > -1 && vScreen[1] > -1 && !iResult)
	{
		vScreen[0] = vScreen[0] * (width / 2) + (width / 2);
		vScreen[1] = -vScreen[1] * (height / 2) + (height / 2);
		return true;
	}
	return false;
}