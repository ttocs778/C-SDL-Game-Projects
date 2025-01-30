#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"

class AnimatedSprite;
class PlayerMove;
class Player : public Actor
{
public:
	Player(Game* game);

private:
	AnimatedSprite* mPlayerAnimation;
	CollisionComponent* mPlayerCollision;
	PlayerMove* mPlayerMove;
	const float PLAYER_SIZE = 32.0f;
};