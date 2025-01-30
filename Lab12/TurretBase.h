#pragma once
#include "Actor.h"

class TurretBase : public Actor
{
private:
	class CollisionComponent* mTurretBaseCollision;
	class MeshComponent* mTurretBaseMesh;
	class TurretHead* mHead;
	class HealthComponent* mTurretHealth;

public:
	TurretBase(Game* game);
	~TurretBase();
	void Die();
};
