#include "DeadFrog.h"
#include "Game.h"

DeadFrog::DeadFrog(Game* game, const Vector2& position)
: Actor(game)
{
	mDeadFrogSprite = new SpriteComponent(this);
	mDeadFrogSprite->SetTexture(mGame->GetTexture("Assets/Dead.png"));
	SetPosition(position);
}

void DeadFrog::OnUpdate(float deltaTime)
{
	mFrogLife += deltaTime;
	if (mFrogLife >= 0.5f)
	{
		SetState(ActorState::Destroy);
	}
}