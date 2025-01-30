#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "WrappingMove.h"
#include "CollisionComponent.h"
#include "Game.h"

class Frog;
class Vehicle : public Actor
{
public:
	Vehicle(Game* game, char type, int row);
	~Vehicle();
	void OnUpdate(float deltaTime) override;

private:
	SpriteComponent* mVehicleSprite;
	WrappingMove* mVehicleMove;
	int mRow = 0;
	CollisionComponent* mVehicleCollision = new CollisionComponent(this);
	const float CAR_SIZE = 32.0f;
	const float TRUCK_WIDTH = 64.0f;
	const float TRUCK_HEIGHT = 24.0f;
	const float NORMAL_SPEED = 50.0f;
	const float SLOW_SPEED = NORMAL_SPEED / 2.0f;
	const float DIRECTION_LEFT = -1.0f;
	const float DIRECTION_RIGHT = 1.0f;
	const float THRESHOLD_ANGLE = Math::Pi / 6.0f;
};