#include "SideBlock.h"
#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"

SideBlock::SideBlock(Game* game, size_t textureIndex)
: Actor(game)
{
	SetScale(500.0f);
	mMc = new MeshComponent(this);
	mMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mMc->SetTextureIndex(textureIndex);
}

void SideBlock::OnUpdate(float deltaTime)
{
	float playerX = mGame->GetPlayer()->GetPosition().x;
	if (GetPosition().x < playerX - 2000.0f)
	{
		SetState(ActorState::Destroy);
	}
}