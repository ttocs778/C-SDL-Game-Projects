#pragma once
#include "MoveComponent.h"

class AnimatedSprite;
class Game;
class CollisionComponent;
class Sword;

enum class MoveDir
{
	Left,
	Right,
	Up,
	Down,
	None
};

class PlayerMove : public MoveComponent
{
public:
	PlayerMove(Actor* owner);
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;
	void StartAttack();

private:
	Game* mGame;
	Vector2 mOffset{-256.0f, -224.0f};
	Vector2 mDirection{0.0f, 0.0f};
	float mSpeed = 150.0f;
	MoveDir mDir;
	AnimatedSprite* mAsc;
	CollisionComponent* mPlayerCollision;
	bool mSpacebarPressedLastFrame = false;
	bool mIsAttack = false;
	float mAttackTime = 0.0f;
	bool mAnimationStarted = false;
	Sword* mSword;
	CollisionComponent* mSwordCollision;
	const float PLAYER_WIDTH = 20.0;
	const float PLAYER_HEIGHT = 28.0;
};