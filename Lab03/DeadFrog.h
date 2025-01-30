#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class DeadFrog : public Actor
{
public:
	DeadFrog(Game* game, const Vector2& position);
	void OnUpdate(float deltaTime) override;

private:
	SpriteComponent* mDeadFrogSprite;
	float mFrogLife = 0.0f;
};