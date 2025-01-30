#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class CollisionComponent;
class AnimatedSprite;
class PlayerMove;
class Collider : public Actor
{
public:
	Collider(Game* game, float width, float height);

private:
	CollisionComponent* mCollider;
};