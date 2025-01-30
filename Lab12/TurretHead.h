#pragma once
#include "Actor.h"
#include "LaserComponent.h"
#include "AudioSystem.h"
#include <map>

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
	void ChangeState(TurretState turretState);
	void CheckForTarget();
	void SelectNewRandomTarget();
	bool CheckForPortalTeleport();
	void Die();
	void TakeDamage();

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
	std::map<TurretState, std::string> mStateSounds;
	SoundHandle mCurrentVOSound;
};
