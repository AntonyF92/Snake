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
	SetCursorPosition(BORDER_X, BORDER_Y);
	unsigned char a = 201;
	std::cout << a;
	SetCursorPosition(BORDER_X, GAME_FIELD_HEIGHT);
	a = 200;
	std::cout << a;
	SetCursorPosition(GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT);
	a = 188;
	std::cout << a;
	SetCursorPosition(GAME_FIELD_WIDTH, BORDER_Y);
	a = 187;
	std::cout << a;
	a = 205;
	for (int i = BORDER_X + 1; i < GAME_FIELD_WIDTH; i++)
	{
		SetCursorPosition(i, BORDER_Y);
		std::cout << a;
		SetCursorPosition(i, GAME_FIELD_HEIGHT);
		std::cout << a;
	}
	a = 186;
	for (int i = BORDER_Y + 1; i < GAME_FIELD_HEIGHT; i++)
	{
		SetCursorPosition(BORDER_X, i);
		std::cout << a;
		SetCursorPosition(GAME_FIELD_WIDTH, i);
		std::cout << a;
	}
	PrintScores(0, 0);
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

void Field::UpdatePlayer(const std::vector<COORD>& forClear, const std::vector<COORD>& forDraw, const bool is_local)
{
	for (auto it = forClear.begin(); it != forClear.end(); it++)
		ClearInPosition(*it);
	if (is_local)
		SetConsoleCharColor(WHITE);
	else
	{
		SetConsoleCharColor(LIGHTGREY);
	}
	for (auto it = forDraw.begin(); it != forDraw.end(); it++)
		PrintSymbolInPos(SNAKE_BODY_SYMBOL, *it);
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
	std::cout << "                                                               " << std::endl;
	SetCursorPosition(0, GAME_FIELD_HEIGHT + 1);
	std::cout << s << std::endl;
}

void Field::PrintScores(int current, int max)
{
	SetConsoleCharColor(WHITE);
	SetCursorPosition(0, 0);
	std::cout << "                               ";
	SetCursorPosition(0, 0);
	std::cout << "Scores: " << current << " Max scores: " << max;
}