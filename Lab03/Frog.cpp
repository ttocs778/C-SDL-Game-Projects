#include "Frog.h"
#include <unordered_map>
#include "Game.h"
#include "DeadFrog.h"
#include "Log.h"

Frog::Frog(Game* game)
: Actor(game)
{
	mFrogSprite = new SpriteComponent(this, 200);
	mFrogSprite->SetTexture(mGame->GetTexture("Assets/Frog.png"));
	mFrogCollision = new CollisionComponent(this);
	mFrogCollision->SetSize(25.0f, 25.0f);
	mKeyLastFrame[SDL_SCANCODE_W] = false;
	mKeyLastFrame[SDL_SCANCODE_A] = false;
	mKeyLastFrame[SDL_SCANCODE_S] = false;
	mKeyLastFrame[SDL_SCANCODE_D] = false;
}

void Frog::OnProcessInput(const Uint8* keyState)
{
	SDL_Scancode keys[] = {SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D};
	for (SDL_Scancode key : keys)
	{
		bool isPressed = keyState[key];
		if (isPressed && !mKeyLastFrame[key])
		{
			if (key == SDL_SCANCODE_W)
			{
				mPosition.y -= MOVE_DISTANCE;
			}
			else if (key == SDL_SCANCODE_A)
			{
				mPosition.x -= MOVE_DISTANCE;
			}
			else if (key == SDL_SCANCODE_S)
			{
				mPosition.y += MOVE_DISTANCE;
			}
			else if (key == SDL_SCANCODE_D)
			{
				mPosition.x += MOVE_DISTANCE;
			}
			mPosition.x = Math::Clamp(mPosition.x, MIN_X, MAX_X);
			mPosition.y = Math::Clamp(mPosition.y, MIN_Y, MAX_Y);
		}
		mKeyLastFrame[key] = isPressed;
	}
}

void Frog::OnUpdate(float deltaTime)
{
	std::vector<Vehicle*> vehicles = mGame->GetVehilce();
	CollisionComponent* frogCollision = GetComponent<CollisionComponent>();
	for (Vehicle* vehilce : vehicles)
	{
		CollisionComponent* vehicleCollision = vehilce->GetComponent<CollisionComponent>();
		if (frogCollision->Intersect(vehicleCollision))
		{
			Vector2 deathPosition = GetPosition();
			new DeadFrog(GetGame(), deathPosition);
			SetPosition(Vector2(FROG_START_X, FROG_START_Y));
		}
	}
	bool isRidingLog = false;
	std::vector<Log*> logs = mGame->GetLog();
	for (Log* log : logs)
	{
		Vector2 offset;
		CollSide side = frogCollision->GetMinOverlap(log->GetComponent<CollisionComponent>(),
													 offset);

		if (side != CollSide::None)
		{
			SetPosition(Vector2(GetPosition().x, log->GetPosition().y));
			Vector2 logDirection = log->GetComponent<WrappingMove>()->GetDirection();
			float logSpeed = log->GetComponent<WrappingMove>()->GetForwardSpeed();
			SetPosition(GetPosition() + logDirection * logSpeed * deltaTime);
			if (side == CollSide::Left)
			{
				SetPosition(Vector2(GetPosition().x + offset.x + 16.0f, GetPosition().y));
			}
			else if (side == CollSide::Right)
			{
				SetPosition(Vector2(GetPosition().x + offset.x - 16.0f, GetPosition().y));
			}
			isRidingLog = true;
			break;
		}
	}
	Actor* goal = mGame->GetGoal();
	if (goal)
	{
		CollisionComponent* goalCollision = goal->GetComponent<CollisionComponent>();
		if (frogCollision->Intersect(goalCollision))
		{
			// Frog reached the goal
			SetPosition(goal->GetPosition());
			SetState(ActorState::Paused);
			return;
		}
	}
	if (!isRidingLog && GetPosition().y >= WATER_MIN_Y && GetPosition().y <= WATER_MAX_Y)
	{
		// If the frog is not riding a log and is in the water area, it dies
		Vector2 deathPosition = GetPosition();
		new DeadFrog(GetGame(), deathPosition);
		SetPosition(Vector2(FROG_START_X, FROG_START_Y)); // Reset to start position
	}

	if (GetPosition().y < WATER_MIN_Y)
	{
		// If the frog is above the water area but not at the goal, it dies
		if (!goal || !frogCollision->Intersect(goal->GetComponent<CollisionComponent>()))
		{
			Vector2 deathPosition = GetPosition();
			new DeadFrog(GetGame(), deathPosition);
			SetPosition(Vector2(FROG_START_X, FROG_START_Y)); // Reset to start position
		}
	}
}