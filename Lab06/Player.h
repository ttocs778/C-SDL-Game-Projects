#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class CollisionComponent;
class AnimatedSprite;
class PlayerMove;
class Player : public Actor
{
public:
	Player(Game* game);

private:
	AnimatedSprite* mPlayerAnimation;
	PlayerMove* mPlayerMove;
	CollisionComponent* mPlayerCollision;
	const float PLAYER_COLL_SIZE = 20.0f;
};