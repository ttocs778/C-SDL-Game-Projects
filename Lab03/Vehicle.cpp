#include "Vehicle.h"
#include "Frog.h"
#include <iostream>

Vehicle::Vehicle(Game* game, char type, int row)
: Actor(game)
, mRow(row)
{
	mVehicleSprite = new SpriteComponent(this);
	std::string texturePath = "Assets/Car";
	if (type == 'T')
	{
		texturePath = "Assets/Truck.png";
		mVehicleCollision->SetSize(TRUCK_WIDTH, TRUCK_HEIGHT);
	}
	else
	{
		// Add the vehicle type to the path
		texturePath += type;
		texturePath += ".png";
		mVehicleCollision->SetSize(CAR_SIZE, CAR_SIZE);
	}
	mVehicleSprite->SetTexture(game->GetTexture(texturePath));
	mVehicleMove = new WrappingMove(this);
	mVehicleMove->SetForwardSpeed(NORMAL_SPEED);
	if (mRow % 2 == 0)
	{
		mVehicleMove->SetDirection(Vector2(DIRECTION_RIGHT, 0.0f));
	}
	else
	{
		mVehicleMove->SetDirection(Vector2(DIRECTION_LEFT, 0.0f));
	}

	// Add vehicle to the game
	mGame->AddVehicle(this);
}

Vehicle::~Vehicle()
{
	mGame->RemoveVehicle(this);
}

void Vehicle::OnUpdate(float deltaTime)
{

	Frog* frog = GetGame()->GetFrog();
	Vector2 frogPosition = frog->GetPosition();
	Vector2 vehiclePosition = GetPosition();
	Vector2 directionToFrog = frogPosition - vehiclePosition;
	Vector2 vehicleForward = mVehicleMove->GetDirection();
	directionToFrog.Normalize();
	vehicleForward.Normalize();
	float dotProduct = Vector2::Dot(vehicleForward, directionToFrog);
	dotProduct = Math::Clamp(dotProduct, DIRECTION_LEFT, DIRECTION_RIGHT);
	// Calculate the angle using inverse cosine
	float angle = Math::Acos(dotProduct);
	if (angle <= THRESHOLD_ANGLE)
	{
		mVehicleMove->SetForwardSpeed(SLOW_SPEED);
	}
	else
	{
		mVehicleMove->SetForwardSpeed(NORMAL_SPEED);
	}
}