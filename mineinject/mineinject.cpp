#include "stdafx.h"

int main()
{
	const char* libPath = "minehook.dll";
	size_t pathLen;
	HANDLE procHandle;
	void* remoteLibPathAddr;
	HANDLE threadHandle;
	HMODULE kernel32Handle;

	pathLen = strlen(libPath);
	if ((procHandle = FindWindow(NULL, L"Minesweeper")) == NULL)
	{
		printf("Could not find winmine process.\n");
		system("PAUSE");

		return 1;
	}
	if ((kernel32Handle = GetModuleHandle(L"Kernel32")) == NULL)
	{
		printf("Could not get Kernel32 module handle.\n");
		system("PAUSE");

		return 1;
	}
	// Allocate memory in WinMine for our DLL's path and write it there.
	remoteLibPathAddr = VirtualAllocEx(procHandle, NULL, pathLen, MEM_COMMIT,
										PAGE_READWRITE);
	if (WriteProcessMemory(procHandle, remoteLibPathAddr, (void*)libPath, pathLen, NULL) == 0)
	{
		printf("Failed to write DLL name. Error Code %d.\n", GetLastError());
		system("PAUSE");
	}
	// Load our DLL into WinMine's process via remote thread.
	threadHandle = CreateRemoteThread(procHandle, NULL, 0,
					(LPTHREAD_START_ROUTINE)GetProcAddress(kernel32Handle, "LoadLibraryA"),
					remoteLibPathAddr, 0, NULL);
	WaitForSingleObject(threadHandle, INFINITE);
	// Cleanup
	CloseHandle(threadHandle);
	VirtualFreeEx(procHandle, remoteLibPathAddr, pathLen, MEM_RELEASE);

    return 0;
}

