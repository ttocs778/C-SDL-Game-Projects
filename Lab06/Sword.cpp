#include "Sword.h"
#include "CollisionComponent.h"

Sword::Sword(Game* game)
: Actor(game)
{
	mSwordCollision = new CollisionComponent(this);
	mSwordCollision->SetSize(SWORD_WIDTH, SWORD_HEIGHT);
}