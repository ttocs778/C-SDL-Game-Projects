#pragma once
#include "Actor.h"
#include "LaserComponent.h"

enum class TurretState
{
	Idle,
	Search,
	Priming,
	Firing,
	Falling,
	Dead
};

class TurretHead : public Actor
{
public:
	TurretHead(Game* game, Actor* parent);
	void OnUpdate(float deltaTime) override;
	void UpdateIdle(float deltaTime);
	void UpdateSearch(float deltaTime);
	void UpdatePriming(float deltaTime);
	void UpdateFiring(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateDead(float deltaTime);
	void ChangeState(TurretState turretState)
	{
		mTurretState = turretState;
		mStateTimer = 0.0f;
	}
	void CheckForTarget();
	void SelectNewRandomTarget();
	bool CheckForPortalTeleport();
	void Die();

private:
	class MeshComponent* mTurretMesh;
	Actor* mActorLaser;
	LaserComponent* mLaser;
	TurretState mTurretState = TurretState::Idle;
	float mStateTimer = 0.0f;
	Actor* mAcquiredTarget = nullptr;
	Vector3 mTargetPoint;
	Quaternion mTargetRotation;
	float mCycleTimer = 0.0f;
	bool mHasTarget = false;
	bool mReturningToCenter = false;
	Vector3 mFallVelocity;
	float mTeleportCooldown = 0.0f;
	float mFireCooldown = 0.0f;
	const float PRIMING_TIME = 1.5f;
	const float FIRE_COOLDOWN = 0.05f;
	const float SEARCH_TIMEOUT = 5.0f;
	const float SIDE_DISTANCE = 75.0f;
	const float UP_DISTANCE = 25.0f;
	const float FORWARD_DISTANCE = 200.0f;
	const float DAMAGE_PER_SHOT = 2.5f;
	const float GRAVITY_ACCELERATION = -980.0f;
	const float TERMINAL_VELOCITY = 800.0f;
	const float FALL_POSITION_OFFSET = 15.0f;
};
