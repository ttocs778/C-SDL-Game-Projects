#include "PlayerMove.h"
#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "AnimatedSprite.h"
#include "Collider.h"
#include "CollisionComponent.h"
#include "EnemyComponent.h"
#include "Sword.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
	mGame = owner->GetGame();
	mDir = MoveDir::None;
	mAsc = owner->GetComponent<AnimatedSprite>();
	mPlayerCollision = owner->GetComponent<CollisionComponent>();
	mSword = new Sword(mGame);
	mSwordCollision = mSword->GetComponent<CollisionComponent>();
}

void PlayerMove::Update(float deltaTime)
{
	Vector2 currentPosition = mOwner->GetPosition();
	Vector2 newPosition = currentPosition;
	const std::vector<Collider*>& colliders = mGame->GetColliders();
	const std::vector<EnemyComponent*>& enemies = mGame->GetEnemies();
	for (EnemyComponent* enemy : enemies)
	{
		CollisionComponent* enemyCollision = enemy->GetOwner()->GetComponent<CollisionComponent>();

		// Check for sword collision if attacking
		if (mIsAttack && mSwordCollision->Intersect(enemyCollision))
		{
			enemy->TakeDamage();
		}

		Vector2 offset;
		CollSide side = mPlayerCollision->GetMinOverlap(
			enemy->GetOwner()->GetComponent<CollisionComponent>(), offset);
		if (side != CollSide::None)
		{
			newPosition += offset;
			mOwner->SetPosition(newPosition);
		}
	}
	for (Collider* collider : colliders)
	{
		Vector2 offset;
		CollSide side =
			mPlayerCollision->GetMinOverlap(collider->GetComponent<CollisionComponent>(), offset);
		if (side != CollSide::None)
		{
			newPosition += offset;
			mOwner->SetPosition(newPosition);
		}
	}
	if (mIsAttack)
	{
		mAttackTime += deltaTime;
		switch (mDir)
		{
		case MoveDir::Up:
			mSword->SetPosition(mOwner->GetPosition() + Vector2{0, -40});
			mSwordCollision->SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
			mAsc->SetAnimation("AttackUp");
			break;
		case MoveDir::Down:
			mSword->SetPosition(mOwner->GetPosition() + Vector2{0, 40});
			mSwordCollision->SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
			mAsc->SetAnimation("AttackDown");
			break;
		case MoveDir::Left:
			mSword->SetPosition(mOwner->GetPosition() + Vector2{-32, 0});
			mSwordCollision->SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
			mAsc->SetAnimation("AttackLeft");
			break;
		case MoveDir::Right:
			mSword->SetPosition(mOwner->GetPosition() + Vector2{32, 0});
			mSwordCollision->SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
			mAsc->SetAnimation("AttackRight");
			break;
		case MoveDir::None:
			break;
		}

		if (mAttackTime > 0.25f)
		{

			mIsAttack = false;
			mAttackTime = 0.0f;
		}
		return;
	}
	newPosition += mDirection * mSpeed * deltaTime;
	mOwner->SetPosition(newPosition);

	if (mDirection.x != 0.0f || mDirection.y != 0.0f)
	{
		switch (mDir)
		{
		case MoveDir::Up:
			mAsc->SetAnimation("WalkUp");
			break;
		case MoveDir::Down:
			mAsc->SetAnimation("WalkDown");
			break;
		case MoveDir::Left:
			mAsc->SetAnimation("WalkLeft");
			break;
		case MoveDir::Right:
			mAsc->SetAnimation("WalkRight");
			break;
		case MoveDir::None:
			break;
		}
	}
	else
	{
		switch (mDir)
		{
		case MoveDir::Up:
			mAsc->SetAnimation("StandUp");
			break;
		case MoveDir::Down:
			mAsc->SetAnimation("StandDown");
			break;
		case MoveDir::Left:
			mAsc->SetAnimation("StandLeft");
			break;
		case MoveDir::Right:
			mAsc->SetAnimation("StandRight");
			break;
		case MoveDir::None:
			break;
		}
	}
	Vector2 playerPos = mOwner->GetPosition();
	Vector2& camPos = mGame->GetCameraPos();
	camPos = playerPos + mOffset;
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	bool isSpacebarPressed = keyState[SDL_SCANCODE_SPACE];
	if (isSpacebarPressed && !mSpacebarPressedLastFrame && !mIsAttack)
	{
		mGame->GetAudio()->PlaySound("SwordSlash.wav");
		mIsAttack = true;
		mAttackTime = 0.0f;
		mAsc->ResetAnimTimer();
	}
	mSpacebarPressedLastFrame = isSpacebarPressed;
	if (!mIsAttack)
	{
		mDirection = Vector2::Zero;
		if (keyState[SDL_SCANCODE_A])
		{
			mDirection.x = -1;
			mDir = MoveDir::Left;
		}
		else if (keyState[SDL_SCANCODE_D])
		{
			mDirection.x = 1;
			mDir = MoveDir::Right;
		}
		else if (keyState[SDL_SCANCODE_W])
		{
			mDirection.y = -1;
			mDir = MoveDir::Up;
		}
		else if (keyState[SDL_SCANCODE_S])
		{
			mDirection.y = 1;
			mDir = MoveDir::Down;
		}
	}
}
