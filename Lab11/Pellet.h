#include "Actor.h"
#pragma once
#include "Game.h"

class Pellet : public Actor
{
public:
	Pellet(Game* game);
	void OnUpdate(float deltaTime) override;
	void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }
	void CalPelletTeleport(Portal* entry, Portal* exit);

private:
	class MeshComponent* mPelletMesh;
	class CollisionComponent* mPelletCollision;
	Vector3 mVelocity;
	float mLifeTime = 0.0f;
	bool mTeleportedRecently = false;
	bool mIsGreen = false;
	const float PELLET_COLL_SIZE = 25.0f;
	const float LIFE_TIME_CAP = 0.25f;
};