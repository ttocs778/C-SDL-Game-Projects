#include "Spawner.h"
#include "Game.h"
#include "Player.h"
#include "Goomba.h"

Spawner::Spawner(Game* game)
: Actor(game)
{
}

void Spawner::OnUpdate(float deltaTime)
{
	Vector2 playerPosition = GetGame()->GetPlayer()->GetPosition();
	Vector2 spawnerPosition = GetPosition();
	float distance = Math::Abs(playerPosition.x - spawnerPosition.x);
	if (distance < DISTANCE)
	{
		Goomba* goomba = new Goomba(mGame);
		goomba->SetPosition(GetPosition());
		SetState(ActorState::Destroy);
	}
}