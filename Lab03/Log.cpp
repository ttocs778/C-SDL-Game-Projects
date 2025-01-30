#include "Log.h"
#include "Game.h"
#include "CollisionComponent.h"

Log::Log(Game* game, char type, int row)
: Actor(game)
, mRow(row)
{
	mLogSprite = new SpriteComponent(this);
	mLogCollision = new CollisionComponent(this);
	switch (type)
	{
	case 'X':
		mLogSprite->SetTexture(mGame->GetTexture("Assets/LogX.png"));
		mLogCollision->SetSize(LOG_X_WIDTH, LOG_HEIGHT);
		break;
	case 'Y':
		mLogSprite->SetTexture(mGame->GetTexture("Assets/LogY.png"));
		mLogCollision->SetSize(LOG_Y_WIDTH, LOG_HEIGHT);
		break;
	case 'Z':
		mLogSprite->SetTexture(mGame->GetTexture("Assets/LogZ.png"));
		mLogCollision->SetSize(LOG_Z_WIDTH, LOG_HEIGHT);
		break;
	}
	mGame->AddLog(this);
	mLogMove = new WrappingMove(this);
	mLogMove->SetForwardSpeed(LOG_SPEED);

	if (mRow % 2 == 0)
	{
		mLogMove->SetDirection(Vector2(DIRECTION_RIGHT, 0.0f));
	}
	else
	{
		mLogMove->SetDirection(Vector2(DIRECTION_LEFT, 0.0f));
	}
}

Log::~Log()
{
	mGame->RemoveLog(this);
}
