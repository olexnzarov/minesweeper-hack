#include "pch.h"
#include <iostream>
#include <Windows.h>

HANDLE GetHandle(const char* windowName)
{
	HWND hwnd = FindWindowA(NULL, windowName);
	if (!hwnd) {
		std::cout << "Couldn't find a window that is called " << windowName << ".\n";
		Sleep(5000);
		exit(1);
	}

	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (!processId) {
		std::cout << "Couldn't get a process id of this window.\n";
		Sleep(5000);
		exit(2);
	}

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (!handle)
	{
		std::cout << "Couldn't open Minesweeper's process.\n";
		Sleep(5000);
		exit(3);
	}

	return handle;
}

int ReadIntFromMemory(HANDLE& handle, int address)
{
	int buffer;
	ReadProcessMemory(handle, (LPCVOID)address, &buffer, sizeof(int), 0);

	return buffer;
}

void FindMines()
{
	HANDLE handle = GetHandle("Minesweeper");

	int fieldWidth = ReadIntFromMemory(handle, 0x01005334);
	int fieldHeight = ReadIntFromMemory(handle, 0x010056a8);
	int totalMines = ReadIntFromMemory(handle, 0x01005330);

	std::cout << "Field Size: " << fieldWidth << "x" << fieldHeight << "\n";
	std::cout << "Mines Count: " << totalMines << "\n\n\n";

	for (int i = 0; i < fieldHeight; i++)
	{
		for (int j = 0; j < fieldWidth; j++)
		{
			char buffer;
			ReadProcessMemory(handle, (LPCVOID) (0x1005340 + (i + 1) * 32 + j + 1), &buffer, sizeof(char), 0);

			std::cout << ((int)buffer == -113 ? "[=]" : "[ ]");

		}

		std::cout << "\n";
	}

	CloseHandle(handle);

	char key;
	std::cout << "\n\nTry again? (y/n) ";
	std::cin >> key;
	if (key == 'y' || key == 'Y')
	{
		std::cout << "\n\n";
		FindMines();
	}
}

int main()
{
	std::cout << "If you press on a mine on your first turn, it will change the layout.\nSo it's best to press on a tile first and then run this program.\n\n";
	FindMines();
	return 0;
}
