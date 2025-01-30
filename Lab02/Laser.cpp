#include "Laser.h"
#include "Game.h"
#include "Random.h"
#include "Math.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
: Actor(game)
, mGame(game)
{
	mSpriteComponent = new SpriteComponent(this);
	mMoveComponent = new MoveComponent(this);

	mSpriteComponent->SetTexture(mGame->GetTexture("Assets/laser.png"));
	mMoveComponent->SetForwardSpeed(400.0f);
}

void Laser::OnUpdate(float deltaTime)
{
	mLaserLife -= deltaTime;
	if (mLaserLife <= 0.0f)
	{
		SetState(ActorState::Destroy);
	}
	if (GetState() == ActorState::Active)
	{
		const std::vector<Asteroid*>& asteroids = mGame->GetAsteroids();
		for (Asteroid* asteroid : asteroids)
		{
			float distance = Vector2::Distance(GetPosition(), asteroid->GetPosition());
			if (distance <= 70.0f)
			{
				SetState(ActorState::Destroy);
				asteroid->SetState(ActorState::Destroy);
				break;
			}
		}
	}
}