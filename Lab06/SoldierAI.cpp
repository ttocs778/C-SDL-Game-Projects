#include "SoldierAI.h"
#include "Actor.h"
#include "Game.h"
#include "PathFinder.h"
#include "PathNode.h"
#include <algorithm>

SoldierAI::SoldierAI(Actor* owner)
: SpriteComponent(owner)
{
	SetIsVisible(false);
	mAsc = mOwner->GetComponent<AnimatedSprite>();
}

void SoldierAI::Setup(PathNode* start, PathNode* end)
{
	mPatrolStart = start;
	mPatrolEnd = end;
	bool pathCal = mOwner->GetGame()->GetPathFinder()->CalculatePath(mPatrolStart, mPatrolEnd,
																	 mPath);
	if (pathCal && !mPath.empty())
	{
		mPrev = mPatrolStart;
		mNext = mPath.back();
		mPath.pop_back();
		UpdateDirection();
	}
}

void SoldierAI::Update(float deltaTime)
{
	if (mIsStunned)
	{
		mStunTimer -= deltaTime;

		// If the stun duration is over, remove the stun state
		if (mStunTimer <= 0.0f)
		{
			mIsStunned = false;
			if (mAsc)
			{
				mAsc->SetIsPaused(false);
			}
		}
		return;
	}

	if (Vector2::Distance(mOwner->GetPosition(), mNext->GetPosition()) <= 1.0f)
	{
		mOwner->SetPosition(mNext->GetPosition());
		if (!mPath.empty())
		{
			mPrev = mNext;
			mNext = mPath.back();
			mPath.pop_back();
			UpdateDirection();
		}
		else
		{
			std::swap(mPatrolStart, mPatrolEnd);
			Setup(mPatrolStart, mPatrolEnd);
		}
	}

	Vector2 newPosition = mOwner->GetPosition() + mDirection * SOLDIER_SPEED * deltaTime;
	mOwner->SetPosition(newPosition);
	UpdateAnimation();
}

void SoldierAI::UpdateDirection()
{
	// Calculate the direction from the current position to the next node
	Vector2 diff = mNext->GetPosition() - mPrev->GetPosition();
	diff.Normalize();
	mDirection = diff;
}

void SoldierAI::UpdateAnimation()
{
	if (mDirection.x == 0)
	{
		if (mDirection.y == 1)
		{
			mAsc->SetAnimation("WalkDown");
		}
		else if (mDirection.y == -1)
		{
			mAsc->SetAnimation("WalkUp");
		}
	}
	else if (mDirection.y == 0)
	{
		if (mDirection.x == 1)
		{
			mAsc->SetAnimation("WalkRight");
		}
		else if (mDirection.x == -1)
		{
			mAsc->SetAnimation("WalkLeft");
		}
	}
}

void SoldierAI::Stun()
{
	mIsStunned = true;
	mStunTimer = STUN_DURATION; // Start the stun timer
	if (mAsc)
	{
		mAsc->SetIsPaused(true); // Pause the animation
	}
}

// This helper is to just debug draw the soldier's path to visualize it
// (only called if this component is set to visible)
void SoldierAI::Draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();

	// Draw from current position to next
	SDL_RenderDrawLine(renderer, static_cast<int>(mOwner->GetPosition().x - cameraPos.x),
					   static_cast<int>(mOwner->GetPosition().y - cameraPos.y),
					   static_cast<int>(mNext->GetPosition().x - cameraPos.x),
					   static_cast<int>(mNext->GetPosition().y - cameraPos.y));

	// Draw from next to first node on path
	if (!mPath.empty())
	{
		// Draw from current position to next
		SDL_RenderDrawLine(renderer, static_cast<int>(mNext->GetPosition().x - cameraPos.x),
						   static_cast<int>(mNext->GetPosition().y - cameraPos.y),
						   static_cast<int>(mPath.back()->GetPosition().x - cameraPos.x),
						   static_cast<int>(mPath.back()->GetPosition().y - cameraPos.y));
	}

	// Draw each node on the path
	if (mPath.size() > 1)
	{
		for (size_t i = 0; i < mPath.size() - 1; i++)
		{
			SDL_RenderDrawLine(renderer, static_cast<int>(mPath[i]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i]->GetPosition().y - cameraPos.y),
							   static_cast<int>(mPath[i + 1]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i + 1]->GetPosition().y - cameraPos.y));
		}
	}
}
