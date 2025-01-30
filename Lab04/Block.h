#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"

class Block : public Actor
{
public:
	Block(Game* game, char type);
	~Block();

private:
	SpriteComponent* mBlockSprite;
	CollisionComponent* mBlockCollision;
	const float BLOCK_SIZE = 32.0f;
};