#include "Asteroid.h"
#include "Game.h"
#include "Random.h"
#include "Math.h"

Asteroid::Asteroid(Game* game)
: Actor(game)
, mGame(game)
{
	mSpriteComponent = new SpriteComponent(this);
	mMoveComponent = new MoveComponent(this);

	mSpriteComponent->SetTexture(mGame->GetTexture("Assets/Asteroid.png"));
	mMoveComponent->SetForwardSpeed(150.0f);

	Vector2 min{0.0f, 0.0f};
	Vector2 max{mGame->SCREEN_WIDTH, mGame->SCREEN_HEIGHT};
	mRotation = Random::GetFloatRange(0.0f, Math::TwoPi);
	mPosition = Random::GetVector(min, max);
	mGame->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	mGame->RemoveAsteroid(this);
}

void Asteroid::OnUpdate(float deltaTime)
{
	if (mPosition.x < 0)
	{
		mPosition.x = mGame->SCREEN_WIDTH - 1;
	}
	else if (mPosition.x > mGame->SCREEN_WIDTH)
	{
		mPosition.x = 0;
	}

	if (mPosition.y < 0)
	{
		mPosition.y = mGame->SCREEN_HEIGHT - 1;
	}
	else if (mPosition.y > mGame->SCREEN_HEIGHT)
	{
		mPosition.y = 0;
	}
	SetPosition(mPosition);
}