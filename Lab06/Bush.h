#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class EnemyComponent;
class CollisionComponent;
class AnimatedSprite;
class PlayerMove;
class Bush : public Actor
{
public:
	Bush(Game* game);

private:
	CollisionComponent* mBushCollision;
	SpriteComponent* mBushSprite;
	EnemyComponent* mBushEnemy;
	const float BUSH_COLL_SIZE = 32.0f;
};