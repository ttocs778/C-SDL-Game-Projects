#pragma once
#include "Actor.h"

class PortalGun : public Actor
{
public:
	PortalGun(Game* game);
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mMeshGun;
	class CollisionComponent* mCollisionGun;
	const float ROTATION_SPEED = Math::Pi;
};