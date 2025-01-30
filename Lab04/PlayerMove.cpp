#include "PlayerMove.h"
#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "Block.h"
#include "Goomba.h"
#include "AnimatedSprite.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
	mGame = owner->GetGame();
}

void PlayerMove::Update(float deltaTime)
{
	// Update player's position based on speed
	Vector2 playerPosition = mOwner->GetPosition();
	playerPosition.x += deltaTime * mForwardSpeed;
	playerPosition.y += deltaTime * mYSpeed;
	mOwner->SetPosition(playerPosition);

	// Handle out of bounds (fall off screen)
	if (playerPosition.y > Game::SCREEN_HEIGHT)
	{
		mOwner->SetState(ActorState::Destroy);
		Mix_HaltChannel(mGame->GetLoopSound());
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Dead.wav"), 0);
		return;
	}

	// Handle winning condition
	if (playerPosition.x >= END_X)
	{
		Mix_HaltChannel(mGame->GetLoopSound());
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/StageClear.wav"), 0);
		mOwner->SetState(ActorState::Paused);
		return;
	}

	// Collision with blocks
	CollisionComponent* playerCollision = mOwner->GetComponent<CollisionComponent>();
	mInAir = true;

	for (Block* block : mGame->GetBlock())
	{
		CollisionComponent* blockCollision = block->GetComponent<CollisionComponent>();
		Vector2 offset;
		CollSide side = playerCollision->GetMinOverlap(blockCollision, offset);

		if (side != CollSide::None)
		{
			// Apply offset uniformly
			playerPosition += offset;
			mOwner->SetPosition(playerPosition);

			// Handle side-specific behavior
			if (side == CollSide::Top)
			{
				mYSpeed = 0.0f;
				mInAir = false;
			}
			else if (side == CollSide::Bottom && mYSpeed < 0.0f)
			{
				mYSpeed = 0.0f;
				Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Bump.wav"), 0);
			}
			else if (side == CollSide::Left || side == CollSide::Right)
			{
				mForwardSpeed = 0.0f;
			}
		}
	}

	// Collision with Goombas
	for (Goomba* goomba : mGame->GetGoomba())
	{
		if (goomba->GetState() != ActorState::Active)
			continue; // Skip inactive Goombas

		CollisionComponent* goombaCollision = goomba->GetComponent<CollisionComponent>();
		Vector2 offset;
		CollSide side = playerCollision->GetMinOverlap(goombaCollision, offset);

		if (side != CollSide::None)
		{
			if (side == CollSide::Top ||
				((side == CollSide::Left || side == CollSide::Right) && mInAir))
			{
				// Stomp the Goomba
				if (!goomba->IsStomped())
				{
					goomba->SetStomped();
					goomba->SetState(ActorState::Paused);
					goomba->GetAnimation()->SetAnimation("dead");
					mYSpeed = HALF_JUMP; // Half-jump
					mInAir = true;
					goomba->SetDestroyTimer(GOOMBA_LIFE);
					Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Stomp.wav"), 0);
				}
			}
			else
			{
				// Mario dies
				Player* mario = static_cast<Player*>(mOwner);
				mario->SetState(ActorState::Paused);
				Mix_HaltChannel(mGame->GetLoopSound());
				Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Dead.wav"), 0);
			}
		}
	}

	// Destroy stomped Goombas after 0.25 seconds
	for (Goomba* goomba : mGame->GetGoomba())
	{
		if (goomba->IsStomped())
		{
			float timer = goomba->GetDestroyTimer();
			timer -= deltaTime;
			goomba->SetDestroyTimer(timer);

			if (timer <= 0.0f)
			{
				goomba->SetState(ActorState::Destroy);
			}
		}
	}

	// Update gravity
	mYSpeed += mGravity * deltaTime;

	// Camera follow logic
	Vector2& cameraPos = mOwner->GetGame()->GetCameraPos();
	Vector2 desiredCameraPos = cameraPos;

	if (playerPosition.x < cameraPos.x)
	{
		playerPosition.x = cameraPos.x; // Prevent player from moving left off-screen
		mOwner->SetPosition(playerPosition);
	}
	else if (playerPosition.x > cameraPos.x + Game::SCREEN_WIDTH / 2.0f)
	{
		desiredCameraPos.x = playerPosition.x - Game::SCREEN_WIDTH / 2.0f;
	}

	if (desiredCameraPos.x > cameraPos.x)
	{
		cameraPos.x = desiredCameraPos.x;
	}

	// Handle player animations
	HandleAnimations();
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	bool isSpacebarPressed = keyState[SDL_SCANCODE_SPACE];
	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		mForwardSpeed = -PLAYER_FORWARD_SPEED;
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		mForwardSpeed = PLAYER_FORWARD_SPEED;
	}
	else
	{
		mForwardSpeed = 0.0f;
	}
	if (isSpacebarPressed && !mSpacebarPressedLastFrame && !mInAir)
	{
		mYSpeed = JUMP_SPEED;
		mInAir = true;
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Jump.wav"), 0);
	}
	mSpacebarPressedLastFrame = isSpacebarPressed;
}

void PlayerMove::HandleAnimations()
{
	AnimatedSprite* playerAnimation = mOwner->GetComponent<AnimatedSprite>();
	std::string currentAnim = playerAnimation->GetAnimName();
	if (mOwner->GetState() == ActorState::Paused)
	{
		playerAnimation->SetAnimation("dead");
	}
	else if (!mInAir)
	{
		if (mForwardSpeed > 0.0f)
		{
			playerAnimation->SetAnimation("runRight");
		}
		else if (mForwardSpeed < 0.0f)
		{
			playerAnimation->SetAnimation("runLeft");
		}
		else
		{
			playerAnimation->SetAnimation("idle");
		}
	}
	else
	{
		if (mForwardSpeed > 0.0f)
		{
			playerAnimation->SetAnimation("jumpRight");
		}
		else if (mForwardSpeed < 0.0f)
		{
			playerAnimation->SetAnimation("jumpLeft");
		}
		else
		{
			if (currentAnim == "runRight" || currentAnim == "jumpRight" || currentAnim == "idle")
			{
				playerAnimation->SetAnimation("jumpRight");
			}
			else
			{
				playerAnimation->SetAnimation("jumpLeft");
			}
		}
	}
}
