#pragma once
#include "MoveComponent.h"

class Game;
class PlayerMove : public MoveComponent
{
public:
	PlayerMove(Actor* owner);
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;
	void HandleAnimations();

private:
	const float PLAYER_FORWARD_SPEED = 300.0;
	bool mInAir = false;
	float mYSpeed = 0.0f;
	Game* mGame;
	bool mSpacebarPressedLastFrame = false;
	const float END_X = 6368.0f;
	const float HALF_JUMP = -350.0f;
	const float GOOMBA_LIFE = 0.25f;
	const float JUMP_SPEED = -700.0f;
	float mGravity = 2000.0f;
};