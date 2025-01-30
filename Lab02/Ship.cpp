#include "Ship.h"
#include "Game.h"
#include "Laser.h"

Ship::Ship(Game* game)
: Actor(game)
, mGame(game)
{
	mSpriteComponent = new SpriteComponent(this);
	mMoveComponent = new MoveComponent(this);

	mSpriteComponent->SetTexture(mGame->GetTexture("Assets/Ship.png"));
	mMoveComponent->SetForwardSpeed(600.0f);
	mMoveComponent->SetAngularSpeed(Math::Pi);
}

void Ship::OnProcessInput(const Uint8* keyState)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;
	if (keyState[SDL_SCANCODE_W])
	{
		forwardSpeed += 300.0f;
		mSpriteComponent->SetTexture(mGame->GetTexture("Assets/ShipThrust.png"));
	}
	if (keyState[SDL_SCANCODE_S])
	{
		forwardSpeed -= 300.0f;
		mSpriteComponent->SetTexture(mGame->GetTexture("Assets/ShipThrust.png"));
	}

	// If neither W nor S is pressed, revert to the normal ship texture
	if (!keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S])
	{
		mSpriteComponent->SetTexture(mGame->GetTexture("Assets/Ship.png"));
	}

	// Set the forward speed after checks
	mMoveComponent->SetForwardSpeed(forwardSpeed);

	// Check A/D keys for rotation
	if (keyState[SDL_SCANCODE_A])
	{
		angularSpeed += Math::Pi;
	}
	if (keyState[SDL_SCANCODE_D])
	{
		angularSpeed -= Math::Pi;
	}

	// Set the angular speed after checks
	mMoveComponent->SetAngularSpeed(angularSpeed);

	// Handle spacebar for firing lasers
	if (keyState[SDL_SCANCODE_SPACE] && (mCoolDown <= 0.0f))
	{
		Laser* laser = new Laser(mGame);
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());
		mCoolDown = 1.0f;
	}
}

void Ship::OnUpdate(float deltaTime)
{
	if (mCoolDown > 0.0f)
	{
		mCoolDown -= deltaTime;
	}
}
