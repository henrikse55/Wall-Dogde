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
#include <string>

#define GETCHANDLE GetStdHandle(STD_OUTPUT_HANDLE);
#define CLEAR_CONSOLE ConsoleHandle::cls();

using namespace std;

enum class Move {Up,Down,Left};

struct Player {
	short posX, posY;
	size_t Score;
};

struct Wall {
	short PosX, PosY;
	size_t height;
};

void KeyHandler();

Player player;
size_t ConsoleHeight;
size_t ConsoleLenght;
short overflowY = 0;
vector<Wall> walls = vector<Wall>();

class ConsoleHandle {
public:

	static void ClearOldLines(short x) {
		cout.flush();
		HANDLE console = getCHandle;
		for (int i = 0; i < ConsoleHeight; i++) {
			COORD pos{ x,i };
			SetConsoleCursorPosition(console, pos);
			cout << ' ';
			cout.flush();
		}
	}

	 static void drawPlayerAtPos(short x, short y, char PlayerChar) {
		 cout.flush();
		 HANDLE console = getCHandle;

		 COORD pos = { x, player.posY -1};
		 SetConsoleCursorPosition(console, pos);
		 cout << ' ';
		 cout.flush();

		 pos = { x, player.posY + 1 };
		 if (!(pos.Y > ConsoleHeight)) {
			SetConsoleCursorPosition(console, pos);
			cout << ' ';
			cout.flush();
		 }

		 pos = { x,y };
		 SetConsoleCursorPosition(console, pos);
		 cout << PlayerChar;
	}
	 static void drawWall(short x, short y, short height, char wallTexture) {
		 cout.flush();
		 HANDLE console = getCHandle;
		 for (short i = 0; i < height; i++) {
			 COORD pos = { x,(y + i)};
			 if (pos.Y > ConsoleHeight) {
				 pos.Y = overflowY;
				 overflowY++;
			 }

			 SetConsoleCursorPosition(console, pos);
			 cout << wallTexture;
		 }
		 overflowY = 0;
	}

	 static void drawText(short x, short y, string text) {
		 cout.flush();
		 HANDLE console = getCHandle;
		 COORD pos{ x,y };
		 SetConsoleCursorPosition(console, pos);
		 cout << text;
	 }

	 static void cls() {
		 static const HANDLE hOut = getCHandle;
		 CONSOLE_SCREEN_BUFFER_INFO csbi;
		 COORD pos{ 0,0 };
		 cout.flush();

		 if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
			 abort();
		 }
		 DWORD lenght = csbi.dwSize.X * csbi.dwSize.Y;
		 DWORD written;

		 FillConsoleOutputCharacter(hOut, TEXT(' '),lenght, pos, &written);
		 FillConsoleOutputAttribute(hOut, csbi.wAttributes, lenght, pos, &written);
		 SetConsoleCursorPosition(hOut, pos);
	 }
};

class Generator {
public:
	static Wall CreateWall() {
		srand(time(NULL));
		size_t wallHeight = rand() % 5 + (ConsoleHeight - 5);
		size_t wallPosY = rand() % ConsoleHeight;
		Wall wall;
		wall.height = wallHeight;
		wall.PosX = ConsoleLenght;
		wall.PosY = wallPosY;
		return wall;
	}
};

class PositionHandle {
public:
	static void CreatePlayer(short posY) {
		player.Score = 0;
		player.posX = 0;
		player.posY = posY;
		
		reDrawPlayer();
		//ConsoleHandle::drawPlayerAtPos(player.posX, player.posY, '>');

		async(KeyHandler);

	}

	static void reDrawPlayer() {
		//CLEAR_CONSOLE;
		ConsoleHandle::drawPlayerAtPos(player.posX, player.posY, '>');
	}

	static void TotalRedraw() {
		CLEAR_CONSOLE;
		reDrawPlayer();
		for (int i = 0; i < walls.size(); i++) {
			ConsoleHandle::drawWall(walls[i].PosX, walls[i].PosY, walls[i].height, '#');
		}
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

	static bool IsPlayerCollided() {
		for (int i = 0; i < walls.size(); i++) {
			if (walls[i].PosX == 0) {
				for (int j = 0; j < walls[i].height; j++) {
					short TotalY = walls[i].PosY + j;
					if (TotalY > ConsoleHeight) {
						if (player.posY == overflowY && player.posX == walls[i].PosX) {
							return true;
						}
						else {
							overflowY++;
						}
					}
					else if (player.posY == TotalY && player.posX == walls[i].PosX) {
						return true;
					}
				}
			}
		}
		return false;
	}

	static void Ticker() {
		size_t SpawnTimer = 0;
		while (1) {
			IncrementWalls();
			future<bool> isPlayerHit = async(IsPlayerCollided);
			if (isPlayerHit.get()) {
				CLEAR_CONSOLE;
				ConsoleHandle::drawText(ConsoleLenght / 2 - 5, ConsoleHeight / 2,  "Please go kill yourself now! Score: " + to_string(player.Score));
				return;
			}
			TotalRedraw();
			if (SpawnTimer > 9) {
				Wall newWall = Generator::CreateWall();
				walls.push_back(newWall);
				SpawnTimer = 0;
			}
			size_t speed = 200 - (player.Score);
			Sleep(speed > 50 ? speed : 50);
			SpawnTimer++;
		}
	}

	static void IncrementWalls() {
		if (walls.size() == 0)
			return;

		for (int i = 0; i < walls.size(); i++) {
			ConsoleHandle::ClearOldLines(walls[i].PosX);
			if (walls[i].PosX == 0)
			{
				walls.erase(walls.begin() + i);
				player.Score++;
				return;
			}
			walls[i].PosX--;
		}
	}
};

void KeyHandler() {
	while (1) {
		if (GetAsyncKeyState(VK_UP)) {
			PositionHandle::MovePlayer(Move::Up);
			PositionHandle::reDrawPlayer();
		}
		else if (GetAsyncKeyState(VK_DOWN)) {
			PositionHandle::MovePlayer(Move::Down);
			PositionHandle::reDrawPlayer();
		}
		if (GetAsyncKeyState(VK_LEFT)) {
			PositionHandle::MovePlayer(Move::Left);
			//PositionHandle::TotalRedraw();
		}
		Sleep(18);
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

	thread ticker(PositionHandle::Ticker);

	PositionHandle::CreatePlayer(csbi.srWindow.Bottom / 2);

	//ConsoleHandle::drawWall(csbi.srWindow.Right, csbi.srWindow.Bottom - 5, 2, '#');

    return 0;
}