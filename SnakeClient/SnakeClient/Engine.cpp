#include <stdafx.h>
#include "Engine.h"

void Engine::Init()
{
	currentTime = 0;
	timeForMove = 0;
	serviceThread = boost::thread(&Engine::FixedUpdate);
}

void Engine::FixedUpdate()
{
	while (true)
	{
		boost::timer timer;
		if (currentTime >= timeForMove)
		{
			MovePlayer(localPlayer);
			timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		}
		boost::this_thread::sleep_for(boost::chrono::microseconds(FIXED_UPDATE_DELTA_TIME));
		currentTime += timer.elapsed() * 1000;
	}
}

void Engine::MovePlayer(LocalPlayer& pl)
{
	switch (pl.CurrentDirection())
	{
	case EDirection::up:
	{

	}
		break;
	}
}