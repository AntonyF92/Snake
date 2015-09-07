#include "GeneralDefinitions.h"
#include <iostream>
#include "Player.h"
#include <Windows.h>

class Field
{
private:
	void SetCursorPosition(int, int);
	void SetConsoleCharColor(int);
public:
	void Init();
	void DrawPlayer(Player*);
	void DrawBonus(COORD);
};