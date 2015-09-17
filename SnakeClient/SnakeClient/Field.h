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
	void UpdatePlayer(const std::vector<COORD>& forClear, const std::vector<COORD>& forDraw, const bool is_local);
	void ClearInPosition(COORD);
	void ClearPlayer(std::vector<COORD>& body);
	void PrintScores(int current, int max);
};