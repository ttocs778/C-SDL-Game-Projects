#pragma once
#include "Actor.h"
#include "EnemyMove.h"

class Enemy : public Actor
{
public:
	Enemy(Game* game);

private:
	class MeshComponent* mEnemyMc;
	EnemyMove* mEnemyMove;
};