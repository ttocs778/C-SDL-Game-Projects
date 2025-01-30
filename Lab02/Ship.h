#pragma once
#include "Actor.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"

class Ship : public Actor
{
public:
	Ship(class Game* game);
	void OnProcessInput(const Uint8* keyState) override;
	void OnUpdate(float deltaTime) override;

protected:
	MoveComponent* mMoveComponent;
	SpriteComponent* mSpriteComponent;
	Game* mGame;
	float mCoolDown = 0.0f;
};