#include "stdafx.h"

extern DWORD jumpBackAddr;

void onAttach()
{
	int hookLength = 6;
	DWORD hookAddress = 0x0100346E; // minesweeper mem address where we want to overwrite

	jumpBackAddr = hookAddress + hookLength; // where to continue minesweeper's execution
	Hook((void*)hookAddress, ourFunct, hookLength);
}

BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		onAttach();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
