#include <stdafx.h>
#include "Field.h"
#include "Constants.h"

void Field::Init()
{
	//HWND hwnd;
	//LPWSTR title = NULL;
	//GetConsoleTitle(title, 1024);
	//hwnd = FindWindow(NULL, title);
	//MoveWindow(hwnd, 0, 0, GAME_FIELD_WIDTH+1, GAME_FIELD_HEIGHT+2, TRUE);//xnew,ynew,wnew,hnew -новые положение x,y, ширина и высота

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = false;
	SetConsoleCursorInfo(consoleHandle, &info);
	SetConsoleCharColor(LIGHTBLUE);
	unsigned char a = 201;
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

void Field::DrawPlayer(Player& pl)
{
	if (pl.IsLocal())
		SetConsoleCharColor(WHITE);
	else
	{
		SetConsoleCharColor(LIGHTGREY);
	}
	for (auto it = pl.Body().begin(); it != pl.Body().end(); it++)
	{
		PrintSymbolInPos(SNAKE_BODY_SYMBOL, *it);
	}
}

void Field::UpdatePlayer(Player& pl)
{
	if (pl.IsLocal())
		SetConsoleCharColor(WHITE);
	else
	{
		SetConsoleCharColor(LIGHTGREY);
	}
	PrintSymbolInPos(SNAKE_BODY_SYMBOL, pl.Body()[0]);
	if (pl.drawRemoveTail)
	{
		Field::ClearInPosition(pl.Body()[pl.Body().size() - 1]);
		pl.RemoveTail();
	}
	else
	{
		pl.drawRemoveTail = true;
	}
}

void Field::ClearPlayer(std::vector<COORD>& body)
{
	for (auto it = body.begin(); it != body.end(); it++)
		ClearInPosition(*it);
}

void Field::ClearInPosition(COORD pos)
{
	PrintSymbolInPos(' ', pos);
}

void Field::PrintSymbolInPos(char symbol, COORD pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	std::cout << symbol;
}

bool Field::CheckHeadPos(COORD& pos)
{
	return ((pos.X != 0) && (pos.X != GAME_FIELD_WIDTH) && (pos.Y != 0) && (pos.Y != GAME_FIELD_HEIGHT));
}

void Field::PrintText(const std::string s)
{
	SetConsoleCharColor(WHITE);
	SetCursorPosition(0, GAME_FIELD_HEIGHT + 1);
	std::cout << s << std::endl;
}