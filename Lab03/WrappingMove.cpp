#include "WrappingMove.h"
#include "Actor.h"
#include "Math.h"
#include "MoveComponent.h"
#include "Game.h"

WrappingMove::WrappingMove(Actor* owner, int updateOrder)
: MoveComponent(owner)
, mDirection(Vector2(0.0f, 0.0f))
{
}

void WrappingMove::Update(float deltaTime)
{
	if (mForwardSpeed != 0.0f)
	{
		Vector2 currentPosition = mOwner->GetPosition();
		Vector2 newPosition = currentPosition + mDirection * mForwardSpeed * deltaTime;
		mOwner->SetPosition(newPosition);
		if (newPosition.x < 0.0f)
		{
			newPosition.x = Game::SCREEN_WIDTH;
		}
		else if (newPosition.x > Game::SCREEN_WIDTH)
		{
			newPosition.x = 0.0f;
		}
		mOwner->SetPosition(newPosition);
	}
}

void WrappingMove::SetDirection(const Vector2& dir)
{
	mDirection = dir;
}
