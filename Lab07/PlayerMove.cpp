#include "PlayerMove.h"
#include "Actor.h"
#include "Math.h"
#include "Renderer.h"
#include "SideBlock.h"
#include "Bullet.h"
#include "CollisionComponent.h"
#include "Block.h"
#include "Player.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
}

void PlayerMove::Update(float deltaTime)
{
	mElapsedTime += deltaTime;
	if (mElapsedTime >= 10.0f)
	{
		mSpeedMultiplier += 0.15f;
		mElapsedTime = 0.0f;
	}
	mVelocity.x = FORWARD_SPEED * mSpeedMultiplier;
	mPlayerPosition += deltaTime * mVelocity;
	mPlayerPosition.y = Math::Clamp(mPlayerPosition.y, PLAYER_Y_MIN, PLAYER_Y_MAX);
	mPlayerPosition.z = Math::Clamp(mPlayerPosition.z, PLAYER_Z_MIN, PLAYER_Z_MAX);
	mOwner->SetPosition(mPlayerPosition);
	Vector3 eye = mPlayerPosition;
	eye.x -= H_DIST;
	eye.z = 0.0f;
	Vector3 targetPos = mPlayerPosition;
	targetPos.x += TARGET_X;
	Matrix4 cameraMatrix = Matrix4::CreateLookAt(eye, targetPos, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(cameraMatrix);
	while (mFurthestX < mPlayerPosition.x + MAX_RANGE)
	{
		SpawnSideBlocksAtX(mFurthestX);
		mFurthestX += BLOCK_DIS;
	}
	Player* player = mOwner->GetGame()->GetPlayer();
	CollisionComponent* playerCollision = player->GetComponent<CollisionComponent>();
	for (Block* block : GetGame()->GetBlocks())
	{
		if (playerCollision->Intersect(block->GetComponent<CollisionComponent>()))
		{
			if (block->GetExplodCheck())
			{
				block->TriggerExplosion();
				player->TakeDamage();
			}
			else
			{
				block->SetState(ActorState::Destroy);
				player->TakeDamage();
			}
		}
	}
	if (mIsRoll)
	{
		mRollDuration += deltaTime;
		float newRollAngle = mOwner->GetGame()->GetPlayer()->GetRollRotation() +
							 8.0f * Math::Pi * deltaTime;
		mOwner->GetGame()->GetPlayer()->SetRollRotation(newRollAngle);
		if (mRollDuration >= 0.5f)
		{
			mOwner->GetGame()->GetPlayer()->SetRollRotation(0.0f);
			mIsRoll = false;
			mRollDuration = 0.0f;
		}
	}
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	mVelocity = Vector3::Zero;
	if (keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S])
	{
		mVelocity.z = Z_VELOCITY * mSpeedMultiplier;
	}
	else if (keyState[SDL_SCANCODE_S] && !keyState[SDL_SCANCODE_W])
	{
		mVelocity.z = -Z_VELOCITY * mSpeedMultiplier;
	}
	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		mVelocity.y = -Z_VELOCITY * mSpeedMultiplier;
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		mVelocity.y = Z_VELOCITY * mSpeedMultiplier;
	}
	bool isSpacebarPressed = keyState[SDL_SCANCODE_SPACE];
	if (isSpacebarPressed && !mSpacebarPressedLastFrame)
	{
		mOwner->GetGame()->GetAudio()->PlaySound("Shoot.wav");
		Vector3 bulletPos = mOwner->GetPosition();
		Bullet* bullet = new Bullet(this->GetGame());
		bullet->GetComponent<MoveComponent>()->SetForwardSpeed(BULLET_SPEED * mSpeedMultiplier);
		bullet->SetPosition(bulletPos);
	}
	mSpacebarPressedLastFrame = isSpacebarPressed;
	bool isQPressed = keyState[SDL_SCANCODE_Q];
	if (isQPressed && !mQPressedLastFrame && !mIsRoll)
	{
		mOwner->GetGame()->GetAudio()->PlaySound("Boost.wav");
		mOwner->GetGame()->GetPlayer()->GetShieldTrack()++;
		mOwner->GetGame()->StopDamageAlert();

		mOwner->GetGame()->GetPlayer()->GetShieldTrack() =
			Math::Clamp(mOwner->GetGame()->GetPlayer()->GetShieldTrack(), 0, 3);
		mIsRoll = true;
		mRollDuration = 0.0f;
	}
	mQPressedLastFrame = isQPressed;
}

void PlayerMove::SpawnSideBlocksAtX(float x)
{
	int leftRightIndex = GetNextLeftRightTextureIndex();
	int topIndex = GetNextTopTextureIndex();

	SideBlock* leftBlock = new SideBlock(this->GetGame(), leftRightIndex);
	leftBlock->SetPosition(Vector3(x, -500.0f, 0.0f));

	// Spawn right block (rotated)
	SideBlock* rightBlock = new SideBlock(this->GetGame(), leftRightIndex);
	rightBlock->SetRotation(Math::Pi);
	rightBlock->SetPosition(Vector3(x, 500.0f, 0.0f));

	// Spawn bottom block
	SideBlock* bottomBlock = new SideBlock(this->GetGame(), 5);
	bottomBlock->SetPosition(Vector3(x, 0.0f, -500.0f));

	// Spawn top block
	SideBlock* topBlock = new SideBlock(this->GetGame(), topIndex);
	topBlock->SetPosition(Vector3(x, 0.0f, 500.0f));
}

int PlayerMove::GetNextLeftRightTextureIndex()
{
	int index = mLeftRightTextureIndex;
	mLeftRightTextureIndex = (mLeftRightTextureIndex + 1) % 4;
	return index % 3;
}

int PlayerMove::GetNextTopTextureIndex()
{
	int index = mTopTextureIndex;
	mTopTextureIndex = (mTopTextureIndex + 1) % 2;
	return 6 + index;
}
