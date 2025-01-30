#include "Bullet.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "CollisionComponent.h"
#include "MoveComponent.h"
#include "Block.h"

Bullet::Bullet(Game* game)
: Actor(game)
{
	SetScale(5.0f);
	mMc = new MeshComponent(this);
	mMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
	mBulletMove = new MoveComponent(this);
	mBulletColl = new CollisionComponent(this);
	mBulletColl->SetSize(10.0f, 10.0f, 10.0f);
}

void Bullet::OnUpdate(float deltaTime)
{
	mLifetime += deltaTime;
	if (mLifetime > 1.0f)
	{
		this->SetState(ActorState::Destroy);
	}
	for (Block* block : mGame->GetBlocks())
	{
		if (mBulletColl->Intersect(block->GetComponent<CollisionComponent>()))
		{
			if (block->GetExplodCheck())
			{
				block->TriggerExplosion();
				this->SetState(ActorState::Destroy);
			}
			else
			{
				this->SetState(ActorState::Destroy);
			}
		}
	}
}