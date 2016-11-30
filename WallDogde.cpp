#include "stdafx.h"

#include <Windows.h>
#pragma comment (lib, "user32.lib")

#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>
#include <stdlib.h>

#define getCHandle GetStdHandle(STD_OUTPUT_HANDLE);
#define CLEAR_CONSOLE system("cls");

using namespace std;

enum class Move {Up,Down,Left};

void KeyHandler();

struct Player {
	short posX, posY;
	size_t Score;
};

struct Wall {
	short PosX, PosY;
	size_t height;
};

Player player;
size_t ConsoleHeight;
size_t ConsoleLenght;
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

	static void RedrawWalls() {
		if (walls.size() == 0)
			return;

		for (int i = 0; i < walls.size(); i++) {
			ConsoleHandle::drawWall(walls[i].PosX, walls[i].PosY, walls[i].height, '#');
		}
	}

	static void TotalRedraw() {
		RedrawPlayer();
		RedrawWalls();
	}

	static void MovePlayer(Move move) {
		switch (move) {
			case Move::Up:
				if (player.posY > 0) {
					player.posY--;
				}
				break;
			case Move::Down:
				if (player.posY < ConsoleHeight) {
					player.posY++;
				}
				break;

			case Move::Left:
				IncrementWalls();
				break;
		}
	}

	static void CheckPlayerAndWalls() {
		//TODO Check Diffrent Statues and positions for the player and walls to see if it has hit the player

	}

	static void IncrementWalls() {
		if (walls.size() == 0)
			return;

		for (int i = 0; i < walls.size(); i++) {
			if (walls[i].PosX == 0)
			{
				walls.erase(walls.begin() + i);
				return;
			}

			walls[i].PosX--;
		}
	}
};

class Generator {
public:
	static Wall CreateWall() {
		srand(time(NULL));
		size_t wallHeight = rand() % 20 + 1;
		size_t wallPosY = rand() % ConsoleHeight;
		Wall wall;
		wall.height = wallHeight;
		wall.PosX = ConsoleLenght;
		wall.PosY = wallPosY;
		return wall;
	}
};

void KeyHandler() {
	while (1) {
		if (GetAsyncKeyState(VK_UP)) {
			//TODO make a "re draw" and move pos
			PositionHandle::MovePlayer(Move::Up);
			PositionHandle::TotalRedraw();
		}
		else if (GetAsyncKeyState(VK_DOWN)) {
			//TODO make a "re draw" and move pos
			PositionHandle::MovePlayer(Move::Down);
			PositionHandle::TotalRedraw();
		}
		else if (GetAsyncKeyState(VK_LEFT)) {
			PositionHandle::MovePlayer(Move::Left);
			PositionHandle::TotalRedraw();
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

	ConsoleHeight = csbi.srWindow.Bottom;
	ConsoleLenght = csbi.srWindow.Right;

	Wall wall = Generator::CreateWall();

	walls.push_back(wall);

	PositionHandle::TotalRedraw();
	
	PositionHandle::CreatePlayer(csbi.srWindow.Bottom / 2);



	//ConsoleHandle::drawWall(csbi.srWindow.Right, csbi.srWindow.Bottom - 5, 2, '#');

    return 0;
}