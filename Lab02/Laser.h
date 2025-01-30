#pragma once
#include "Actor.h"
#include <vector>
#include "MoveComponent.h"
#include "SpriteComponent.h"

class Laser : public Actor
{
public:
	Laser(class Game* game);
	void OnUpdate(float deltaTime) override;

protected:
	MoveComponent* mMoveComponent;
	SpriteComponent* mSpriteComponent;
	Game* mGame;
	float mLaserLife = 1.0f;
};