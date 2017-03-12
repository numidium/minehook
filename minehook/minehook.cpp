#include "stdafx.h"

bool Hook(void* toHook, void* ourFunct, int len)
{
	DWORD oldProtection;
	DWORD relativeAddress;
	DWORD temp;

	if (len < 5)
	{
		return false;
	}
	// Relative address is the distance between the target code and our code.
	// 5 is subtracted because the jump is relative to the end of our jump.
	// Since we want to jump to the beginning of our procedure, we need to
	// go back 5 bytes, which is the length of the JMP instruction from
	// which we are jumping.
	relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &oldProtection);
	// write NOP sled
	memset(toHook, 0x90, len);
	// write jump instruction over NOPs
	*(BYTE*)toHook = 0xE9; // E9 = JMP
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress; // relative address to jump to
	VirtualProtect(toHook, len, oldProtection, &temp);

	return true;
}

DWORD jumpBackAddr;

void __declspec(naked) ourFunct()
{
	int flagDelta;

	_asm {
		// execute minesweeper code that was overwritten for the jump
		add ecx, ecx
		mov edx, [ebp-8]
		add dword ptr ds:[0x1005194], eax
		// If eax is -1, that means minesweeper was about to decrement
		// the mine count. If minesweeper wants to decrement the mine
		// count, then that means the user just placed a flag.
		mov flagDelta, eax
	}
	if (flagDelta < 0)
	{
		MessageBox(NULL, TEXT("Flag placed."), TEXT("Hello"), MB_OK);
	}
	_asm {
		jmp[jumpBackAddr] // return control to minesweeper
	}
}