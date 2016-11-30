#include "stdafx.h"

#include <Windows.h>
#pragma comment (lib, "user32.lib")

#include <iostream>
#include <future>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h>

#define getCHandle GetStdHandle(STD_OUTPUT_HANDLE);
#define CLEAR_CONSOLE system("cls");

using namespace std;

enum class Move {Up,Down};

void KeyHandler();

struct Player {
	size_t posX, posY;
	size_t Score;
};

struct Wall {
	size_t PosX, PosY;
	size_t height;
};

Player player;
size_t posYLimit;
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
		
		ConsoleHandle::drawPlayerAtPos(player.posX, player.posY, '>');

		async(KeyHandler);

	}

	static void RedrawPlayer() {
		CLEAR_CONSOLE;
		ConsoleHandle::drawPlayerAtPos(player.posX, player.posY, '>');
	}

	static void MovePlayer(Move move) {
		switch (move) {
			case Move::Up:
				if (player.posY != 0) {
					player.posY--;
				}
				else {
					player.posY = 0;
				}
				break;
			case Move::Down:
				if (player.posY < posYLimit) {
					player.posY++;
				}
				break;
		}
	}

	void IncrementWalls() {

	}
};

class Generator {
public:
	static Wall CreateWall() {
		//TODO Make Generation Method
	}
};

void KeyHandler() {
	while (1) {
		if (GetAsyncKeyState(VK_UP)) {
			//TODO make a "re draw" and move pos
			PositionHandle::MovePlayer(Move::Up);
			PositionHandle::RedrawPlayer();
		}
		else if (GetAsyncKeyState(VK_DOWN)) {
			//TODO make a "re draw" and move pos
			PositionHandle::MovePlayer(Move::Down);
			PositionHandle::RedrawPlayer();
		}
		Sleep(17);
	}
}

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

	posYLimit = csbi.srWindow.Bottom;

	PositionHandle::CreatePlayer(csbi.srWindow.Bottom / 2);

	//ConsoleHandle::drawWall(csbi.srWindow.Right, csbi.srWindow.Bottom - 5, 2, '#');

    return 0;
}