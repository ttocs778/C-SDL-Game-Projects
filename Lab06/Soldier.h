#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class PathNode;
class CollisionComponent;
class AnimatedSprite;
class PlayerMove;
class EnemyComponent;
class Soldier : public Actor
{
public:
	Soldier(Game* game, PathNode* start, PathNode* end);

private:
	CollisionComponent* mSoldierCollision;
	AnimatedSprite* mSoldierAnimation;
	EnemyComponent* mSoldierEnemy;
	const float SOLDIER_COLL_SIZE = 32.0f;
};