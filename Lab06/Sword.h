#pragma once
#include "Actor.h"

class Sword : public Actor
{
public:
	Sword(Game* game);

private:
	class CollisionComponent* mSwordCollision;
	const float SWORD_WIDTH = 28.0;
	const float SWORD_HEIGHT = 28.0;
};