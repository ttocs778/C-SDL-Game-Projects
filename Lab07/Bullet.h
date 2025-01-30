#pragma once
#include "Actor.h"

class Bullet : public Actor
{
public:
	Bullet(Game* game);
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mMc;
	class CollisionComponent* mBulletColl;
	class MoveComponent* mBulletMove;
	float mLifetime = 0.0f;
};