#include <stdafx.h>
#include "Engine.h"

void Engine::Init()
{
	gameState = EGameState::in_progress;
	currentTime = 0;
	timeForMove = 0;
	field.Init();
	canChangeDirection = true;
	localPlayer = LocalPlayer();
	field.DrawPlayer(localPlayer);
	serviceThread = boost::thread(boost::bind(&Engine::FixedUpdate, this));
}

void Engine::FixedUpdate()
{
	while (true)
	{
		boost::timer timer;
		if (currentTime >= timeForMove)
		{
			boost::lock_guard<boost::mutex> lock(lpMutex);
			auto old = localPlayer.OldBody();
			MovePlayer(localPlayer);
			field.ClearPlayer(old);
			field.DrawPlayer(localPlayer);
			for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
			{
				old = it->OldBody();
				it->Move(it->CurrentDirection());
				field.ClearPlayer(old);
				field.DrawPlayer(*it);
			}
			canChangeDirection = true;
			timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		}
		if (gameState != EGameState::in_progress)
		{
			field.PrintText("Game ended! Press Esc for exit...");
			break;
		}
		currentTime += timer.elapsed() * 1000;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
		currentTime += FIXED_UPDATE_DELTA_TIME;
	}
	TerminateThread(serviceThread.native_handle(), 0);
}

EGameState Engine::GameState() const
{
	return gameState;
}

void Engine::MovePlayer(LocalPlayer& pl)
{
	COORD newPos = pl.HeadPosition();
	switch (pl.CurrentDirection())
	{
	case EDirection::up: newPos.Y--; break;
	case EDirection::down: newPos.Y++; break;
	case EDirection::left: newPos.X--; break;
	case EDirection::right: newPos.X++; break;
	}
	if (Engine::CheckPosForBonus(newPos))
		pl.AddBlock(SnakeBlock(0, newPos));
	else
		pl.Move(pl.CurrentDirection());
	if (!field.CheckHeadPos(pl.HeadPosition()) || !localPlayer.CheckCollision(pl.HeadPosition()))
		gameState = EGameState::lose;
	
}

void Engine::SetLocalDirection(EDirection direction)
{
	boost::lock_guard<boost::mutex> lock(lpMutex);
	if (canChangeDirection)
	{
		canChangeDirection = !localPlayer.SetDirection(direction);
	}
}

bool Engine::CheckPosForBonus(COORD& pos)
{
	for (auto it = bonusList.begin();it!=bonusList.end();it++)
	if (it->X == pos.X && it->Y == pos.Y)
		return true;
	return false;
}