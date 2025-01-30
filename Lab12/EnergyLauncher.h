#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class EnergyLauncher : public Actor
{
public:
	EnergyLauncher(Game* game, float cooldown, const std::string& doorName);
	~EnergyLauncher();
	void OnUpdate(float deltaTime) override;

private:
	class CollisionComponent* mEnergyLauncherCollision;
	class MeshComponent* mEnergyLauncherMesh;
	float mCoolDown;
	class Pellet* mPellet = nullptr;
	float mElapsedTimes = 0.0f;
	Vector3 mPelletVelocity;
	std::string mDoorName;
	const float LAUNCHER_COLL_SIZE = 50.0f;
	const float PELLET_OFFSET = 20.0f;
	const float PELLET_SPEED = 500.0f;
};