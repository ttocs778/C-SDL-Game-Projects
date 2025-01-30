#include "Game.h"
#include "Collider.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"

Collider::Collider(Game* game, float width, float height)
: Actor(game)
{
	mCollider = new CollisionComponent(this);
	mCollider->SetSize(width, height);
}