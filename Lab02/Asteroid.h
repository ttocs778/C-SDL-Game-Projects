#pragma once
#include "Actor.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"

class Asteroid : public Actor
{
public:
	Asteroid(class Game* game);
	virtual ~Asteroid();
	void OnUpdate(float deltaTime) override;

protected:
	MoveComponent* mMoveComponent;
	SpriteComponent* mSpriteComponent;
	Game* mGame;
};