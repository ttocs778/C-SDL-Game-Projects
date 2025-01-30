#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "CollisionComponent.h"

class PlayerMove : public MoveComponent
{
public:
	enum MoveState
	{
		OnGround,
		Jump,
		Falling
	};
	PlayerMove(class Actor* owner);
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
					  const Vector2& relativeMouse) override;
	void CreatePortal(bool isBlue);
	void UpdateCrosshairState();
	void ResetPortals();
	const Vector3& GetAcc() const { return mAcceleration; }
	const Vector3& GetVel() const { return mVelocity; }

private:
	MoveState mCurrentState = OnGround;
	void ChangeState(MoveState state) { mCurrentState = state; }
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force) { mPendingForces += force; };
	void FixXYVelocity();
	CollSide FixCollision(CollisionComponent* self, CollisionComponent* collider);
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	const float MOVE_SCALE = 700.0f;
	const float MAX_SPEED = 400.0f;
	const float BRAKING_FACTOR = 0.9f;
	Vector3 mGravity{0.0f, 0.0f, -980.0f};
	Vector3 mJumpForce{0.0f, 0.0f, 35000.0f};
	class Crosshair* mCrosshair;
	class Portal* mPortal = nullptr;
	bool mIsLeftMousePressed = false;
	bool mIsRightMousePressed = false;
	bool mIsRPressed = false;

	class Portal* mBluePortal = nullptr;
	class Portal* mOrangePortal = nullptr;
};
