#include "GeneralDefinitions.h"
#include <iostream>
#include "Player.h"
#include <Windows.h>

class Field
{
private:
	void SetCursorPosition(int, int);
	void SetConsoleCharColor(int);
	void PrintSymbolInPos(char, COORD);
public:
	void Init();
	void DrawPlayer(Player&);
	void DrawBonus(COORD);
	bool CheckHeadPos(COORD&);
	void PrintText(const std::string);
	void UpdatePlayer(Player&);
	void ClearInPosition(COORD);
	void ClearPlayer(std::vector<SnakeBlock>& body);
};