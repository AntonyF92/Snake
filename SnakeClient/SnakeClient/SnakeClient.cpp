// SnakeClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"
#include <conio.h>
#include <exception>
#include "boost\exception\all.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	Engine engine;
	try
	{
		engine.Init();
		char key;
		while ((key = _getch()))
		{
			if (key == 27)
				return 0;
			switch (key)
			{
			case 72: engine.SetLocalDirection(EDirection::up); break;
			case 80: engine.SetLocalDirection(EDirection::down); break;
			case 77: engine.SetLocalDirection(EDirection::right); break;
			case 75: engine.SetLocalDirection(EDirection::left); break;
			}
		}
	}
	catch (std::exception& e)
	{
		engine.PrintString(e.what());
	}
	catch (boost::exception& e)
	{
		engine.PrintString(boost::diagnostic_information_what(e));
	}
	system("pause");
	return 0;
}

