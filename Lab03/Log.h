#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "WrappingMove.h"

class CollisionComponent;
class Log : public Actor
{
public:
	Log(Game* game, char type, int row);
	~Log();

private:
	SpriteComponent* mLogSprite;
	WrappingMove* mLogMove;
	int mRow = 0;
	CollisionComponent* mLogCollision;
	const float LOG_X_WIDTH = 96.0f;
	const float LOG_Y_WIDTH = 128.0f;
	const float LOG_Z_WIDTH = 192.0f;
	const float LOG_HEIGHT = 24.0f;
	const float LOG_SPEED = 37.5f;
	const float DIRECTION_LEFT = -1.0f;
	const float DIRECTION_RIGHT = 1.0f;
};