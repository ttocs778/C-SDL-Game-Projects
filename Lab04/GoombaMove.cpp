#include "GoombaMove.h"
#include "Game.h"
#include "Goomba.h"
#include "Block.h"
#include "AnimatedSprite.h"

GoombaMove ::GoombaMove(Actor* owner)
: MoveComponent(owner)
{
}

void GoombaMove::Update(float deltaTime)
{
	Vector2 goombaPosition = mOwner->GetPosition();
	goombaPosition.x += mForwardSpeed * deltaTime;
	goombaPosition.y += mYSpeed * deltaTime;

	if (goombaPosition.y >= Game::SCREEN_HEIGHT)
	{
		goombaPosition.y = Game::SCREEN_HEIGHT;
		mYSpeed = 0.0f;
	}
	std::vector<Goomba*> goombas = mOwner->GetGame()->GetGoomba();
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlock();
	mOwner->SetPosition(goombaPosition);
	CollisionComponent* goombaCollision = mOwner->GetComponent<CollisionComponent>();
	for (Block* block : blocks)
	{
		CollisionComponent* blockCollision = block->GetComponent<CollisionComponent>();
		Vector2 offset;
		CollSide side = goombaCollision->GetMinOverlap(blockCollision, offset);
		if (side != CollSide::None)
		{
			goombaPosition.y += offset.y;
			if (side == CollSide::Top)
			{
				mYSpeed = 0.0f;
			}
		}
	}
	mOwner->SetPosition(goombaPosition);
	mYSpeed += mGravity * deltaTime;

	if (goombaPosition.y >= Game::SCREEN_HEIGHT)
	{
		mOwner->SetState(ActorState::Destroy);
	}

	for (Block* block : blocks)
	{
		CollisionComponent* blockCollision = block->GetComponent<CollisionComponent>();
		Vector2 offset;
		CollSide side = goombaCollision->GetMinOverlap(blockCollision, offset);
		if (side != CollSide::None)
		{
			goombaPosition.y += offset.y;
			if (side == CollSide::Right || side == CollSide::Left)
			{
				mForwardSpeed *= -1;
				goombaPosition.x += mForwardSpeed * deltaTime;
			}
		}
	}
	for (Goomba* goomba : goombas)
	{
		if (goomba != mOwner)
		{
			CollisionComponent* otherGoombaCollision = goomba->GetComponent<CollisionComponent>();
			Vector2 offset;
			CollSide side = goombaCollision->GetMinOverlap(otherGoombaCollision, offset);
			if (side != CollSide::None)
			{
				goombaPosition.y += offset.y;
				if (side == CollSide::Left || side == CollSide::Right)
				{
					mForwardSpeed *= -1;
					goombaPosition.x += mForwardSpeed * deltaTime;
				}
			}
		}
	}
}
