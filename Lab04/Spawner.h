#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "Math.h"

class Spawner : public Actor
{
public:
	Spawner(Game* game);
	void OnUpdate(float deltaTime) override;

private:
	const float DISTANCE = 600.0f;
};