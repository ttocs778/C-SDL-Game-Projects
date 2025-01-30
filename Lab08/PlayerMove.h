#pragma once
#include "VehicleMove.h"
#include "Game.h"

class PlayerMove : public VehicleMove
{
public:
	PlayerMove(class Actor* owner);
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;
	void OnLapChange(int newLap) override;
};