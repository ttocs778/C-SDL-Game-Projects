#include "Block.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "CollisionComponent.h"
#include "Player.h"

Block::Block(Game* game, BlockType type)
: Actor(game)
{
	SetScale(25.0f);
	mMc = new MeshComponent(this);
	mMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mBlockColl = new CollisionComponent(this);
	mBlockColl->SetSize(1.0, 1.0, 1.0);
	mGame->AddBlock(this);
	if (type == Exploding)
	{
		mMc->SetTextureIndex(4);
		mIsExploding = true;
	}
	else
	{
		mMc->SetTextureIndex(3);
	}
}

Block::~Block()
{
	mGame->RemoveBlock(this);
}

void Block::OnUpdate(float deltaTime)
{
	Vector3 playerPos = GetGame()->GetPlayer()->GetPosition();
	if (GetPosition().x < playerPos.x - 2000.0f)
	{
		SetState(ActorState::Destroy);
	}
}

void Block::TriggerExplosion()
{
	if (!mGame->HasExplosionSoundPlayed())
	{

		mGame->GetAudio()->PlaySound("BlockExplode.wav");
		mGame->SetExplosionSoundPlayed(true);
	}
	if (mHasExploded)
	{
		return;
	}
	mHasExploded = true;
	SetState(ActorState::Destroy);
	float explosionRadius = 50.0f;
	for (Block* block : GetGame()->GetBlocks())
	{
		if (block == this)
			continue; // Skip self

		float distance = (block->GetPosition() - GetPosition()).Length();
		if (distance <= explosionRadius)
		{
			if (block->GetExplodCheck())
			{
				block->TriggerExplosion();
			}
			else
			{
				block->SetState(ActorState::Destroy);
			}
		}
	}
}