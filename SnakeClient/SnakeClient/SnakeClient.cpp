// SnakeClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"
#include <conio.h>
#include <exception>
#include "boost\exception\all.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		Engine engine;
		engine.Init();
		char key;
		while ((key = _getch()))
		{
			if (key == 27)
				break;
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
		std::cout << e.what() << std::endl;
	}
	catch (boost::exception& e)
	{
		std::cout << boost::diagnostic_information_what(e) << std::endl;
	}
	return 0;
}

