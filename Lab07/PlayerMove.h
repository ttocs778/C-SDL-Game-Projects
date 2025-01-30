#pragma once
#include "MoveComponent.h"
#include "Game.h"

class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Actor* owner);
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;
	void SpawnSideBlocksAtX(float x);
	int GetNextLeftRightTextureIndex();
	int GetNextTopTextureIndex();
	void TakeDamage();

private:
	Vector3 mVelocity;
	Vector3 mPlayerPosition = mOwner->GetPosition();
	int mLeftRightTextureIndex = 0;
	int mTopTextureIndex = 0;
	float mFurthestX = 0.0f;
	bool mSpacebarPressedLastFrame = false;
	bool mQPressedLastFrame = false;
	float mSpeedMultiplier = 1.0f;
	float mElapsedTime = 0.0f;
	bool mIsRoll = false;
	float mRollDuration = 0.0;
	const float FORWARD_SPEED = 400.0f;
	const float PLAYER_Y_MIN = -180.0f;
	const float PLAYER_Y_MAX = 180.0f;
	const float PLAYER_Z_MIN = -225.0f;
	const float PLAYER_Z_MAX = 225.0f;
	const float H_DIST = 300.0f;
	const float TARGET_X = 20.0f;
	const float MAX_RANGE = 4000.0f;
	const float BLOCK_DIS = 500.0f;
	const float Z_VELOCITY = 300.0f;
	const float BULLET_SPEED = 900.0f;
};