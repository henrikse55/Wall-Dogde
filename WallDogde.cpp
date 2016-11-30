#include "stdafx.h"

#include <Windows.h>
#pragma comment (lib, "user32.lib")

#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h>

#define getCHandle GetStdHandle(STD_OUTPUT_HANDLE);
#define CLEAR_CONSOLE system("cls");

using namespace std;

struct Player {
	size_t posX, posY;
	size_t Score;
};

struct Wall {
	size_t PosX, PosY;
	size_t height;
};

Player player;
vector<Wall> walls = vector<Wall>();

class ConsoleHandle {
public:
	 static void drawPlayerAtPos(short x, short y, char player) {
		 HANDLE console = getCHandle;
		 COORD pos = { x,y };
		 SetConsoleCursorPosition(console, pos);
		 cout << player;
	}

	 static void drawWall(short x, short y, short height, char wallTexture) {
		 HANDLE console = getCHandle;
		 for (short i = height; i > 0; i--) {
			 COORD pos = { x,(y - i)};
			 SetConsoleCursorPosition(console, pos);
			 cout << wallTexture;
		 }
	}
};

class PositionHandle {
public:
	static void CreatePlayer(short posY) {
		player.Score = 0;
		player.posX = 0;
		player.posY = posY;

		//TODO Start Movement Threads
		ConsoleHandle::drawPlayerAtPos(player.posX, player.posY, '>');
	}

	void IncrementWalls() {

	}
};

class Generator {
public:
	static Wall CreateWall() {

	}
};

int main()
{
	//Get and Set Cursor Info
	HANDLE console = getCHandle;
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(console, &CursorInfo);
	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(console, &CursorInfo);
	//-END-//

	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(console, &csbi);

	PositionHandle::CreatePlayer(csbi.srWindow.Bottom - 5);

	ConsoleHandle::drawWall(csbi.srWindow.Right, csbi.srWindow.Bottom - 5, 2, '#');

	cin.ignore();
    return 0;
}