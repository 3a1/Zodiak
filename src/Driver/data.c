#include "data.h"

/* Def values */
bool bEsp = true;
bool bAimbot = true;
bool bOnlyEnemies = false;

void DataInitialize() 
{
	/* Getting hw.dll base address */
	if (!get_hw_base())
	{
		DebugPrint("hw.dll not found");
		return;
	}
	DebugPrint("hw.dll -> 0x%08X", hw_dll);

	/* Getting client.dll base address */
	if (!get_client_base())
	{
		DebugPrint("client.dll not found");
		return;
	}
	DebugPrint("client.dll -> 0x%08X", client_dll);

	/* Getting screen size */
	SpoofContext();
	GetScreenResolution(&screen_size); // for this function we need spoof context first
	RestoreContext();

	DataLoop();
}

void DataLoop()
{
	while (true)
	{
		/* Clearing entity list */
		clear_entity_list();

		/* Reading view matrix */
		if (!read_view_matrix()) 
		{
			return; // used as a check to see if the game is running
		}

		/* Spoofing our process context to the win32 context */
		SpoofContext();

		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			/* Dead Check */
			if (entity_dead(i))
			{
				continue;
			}

			if (bOnlyEnemies) 
			{
				if (entity_teammate(i))
				{
					continue;
				}
			}

			/* Making entity screen transformation */
			BOX box;
			if (!entity_screen(i, &box))
			{
				continue;
			}

			/* Drawing esp box */
			if (bEsp) 
			{
				DrawRect((LONG)box.x, (LONG)box.y, (LONG)box.w, (LONG)box.h, (unsigned char)box_color.r, (unsigned char)box_color.g, (unsigned char)box_color.b);
			}
		}

		/* Handle Key Inputs */
		HandleKeyInputs();

		/* Bringing back original context */
		RestoreContext();

		/* Make sleep do decrease cpu usage */
		NtSleep(1);
	}
}