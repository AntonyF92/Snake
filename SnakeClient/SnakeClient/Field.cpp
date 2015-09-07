#include <stdafx.h>
#include "Field.h"
#include "Constants.h"

void Field::Init()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = false;
	SetConsoleCursorInfo(consoleHandle, &info);
	SetConsoleCharColor(LIGHTBLUE);
	char a = 201;
	std::cout << a;
	SetCursorPosition(0, GAME_FIELD_HEIGHT);
	a = 200;
	std::cout << a;
	SetCursorPosition(GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT);
	a = 188;
	std::cout << a;
	SetCursorPosition(GAME_FIELD_WIDTH, 0);
	a = 187;
	std::cout << a;
	a = 205;
	for (int i = 1; i < GAME_FIELD_WIDTH; i++)
	{
		SetCursorPosition(i, 0);
		std::cout << a;
		SetCursorPosition(i, GAME_FIELD_HEIGHT);
		std::cout << a;
	}
	a = 186;
	for (int i = 1; i < GAME_FIELD_HEIGHT; i++)
	{
		SetCursorPosition(0, i);
		std::cout << a;
		SetCursorPosition(GAME_FIELD_WIDTH, i);
		std::cout << a;
	}
}

void Field::SetCursorPosition(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Field::SetConsoleCharColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Field::DrawBonus(COORD pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	SetConsoleCharColor(GREEN);
	std::cout << BONUS_SYMBOL;
}

void Field::DrawPlayer(Player* pl)
{
	for (auto it = pl->Body().begin(); it != pl->Body().end(); it++)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), it->pos);
		if (pl->IsLocal())
			SetConsoleCharColor(WHITE);
		else
		{
			SetConsoleCharColor(LIGHTGREY);
		}
		std::cout << SNAKE_BODY_SYMBOL; 
	}
}